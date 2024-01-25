#include <algorithm>
#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <cstddef>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <unordered_map>

enum class Trap : uint32_t
{
    NONE,        // The CPU is not trapped.
    HALT,        // The CPU has been requested to halt.
    BAD_ADDRESS, // The CPU attempted to execute code from a bad address.
};

// Enough of a CPU for experimental purposes.
//
// Instructions supported:
//
// add  - rd <- rs1 + rs2
// addi - rd <- rs1 + imm32
// bne  - pc <- pc + imm32 if rs1 != rs2 else pc + 1
// beq  - pc <- pc + imm32 if rs1 == rs2 else pc + 1
// jmp  - pc <- rs1 + rs2 + imm32

struct Cpu
{
    uint32_t pc;           // The address of the current instruction.
    uint32_t nextPc;       // The address of the next instruction.
    uint32_t xreg[32];     // Integer registers.
    Trap trap{Trap::NONE}; // Set to a value other than Trap::NONE if the CPU is trapped.
};

using Reg = uint32_t;

namespace vm
{
    enum Opcode
    {
        TRAP,
        ADD,
        ADDI,
        BNE,
        BEQ,
        JMP,
    };

    struct Reg3
    {
        Reg r1;
        Reg r2;
        Reg r3;
    };

    struct Reg2Imm
    {
        Reg r1;
        Reg r2;
        int32_t imm;
    };

    struct Instruction
    {
        Opcode op;
        union
        {
            Trap trap;
            struct Reg3 reg3;
            struct Reg2Imm reg2imm;
        };
    };

    using Code = std::vector<Instruction>;

    class Assembler
    {
        Code& code_;

    public:
        Assembler(Code& code) : code_{code} {}

        auto Trap(Trap trap) -> void { code_.emplace_back(Instruction{.op = TRAP, .trap = trap}); }
        auto Add(Reg rd, Reg rs1, Reg rs2) -> void { code_.emplace_back(Instruction{.op = ADD, .reg3 = {.r1 = rd, .r2 = rs1, .r3 = rs2}}); }
        auto Addi(Reg rd, Reg rs1, int32_t imm) -> void { code_.emplace_back(Instruction{.op = ADDI, .reg2imm = {.r1 = rd, .r2 = rs1, .imm = imm}}); }
        auto Bne(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BNE, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
        auto Beq(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BEQ, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
        auto Jmp(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = JMP, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
    };
} // namespace vm

// How we invoke a function that works on the CPU.
using CpuFunc = uintptr_t (*)(Cpu*, uint32_t);

// Let's assume WIN32, x64 for now. Calling convention is that the first argument is in rcx and the second is in rdx.
constexpr asmjit::x86::Gp ARG0 = asmjit::x86::rcx; // First argument.
constexpr asmjit::x86::Gp ARG1 = asmjit::x86::rdx; // Second argument.

inline auto XregOfs(Reg r) -> asmjit::x86::Mem
{
    // What's the address of the given xregister relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, xreg[r])));
}

inline auto PcOfs() -> asmjit::x86::Mem
{
    // What's the address of pc relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, pc)));
}

inline auto NextPcOfs() -> asmjit::x86::Mem
{
    // What's the address of nextPc relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, nextPc)));
}

inline auto TrapOfs() -> asmjit::x86::Mem
{
    // What's the address of trap relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, trap)));
}

class OffsetMap
{
    struct Entry
    {
        std::uint8_t pc;     // pc relative to previous entry.
        std::uint8_t offset; // native offset relative to previous entry.
    };

    uint32_t startPc_{};         // The VM address of the start of the function.
    uint32_t lastPc_{};          // The VM address of the last entry to be written to the map.
    uint64_t lastOffset_{};      // The native offset of the last entry to be written to the map.
    std::vector<Entry> entries_; // Relative offsets from the previous entry.

public:
    OffsetMap(uint32_t startPc) : startPc_{startPc}, lastPc_{startPc} {}

    auto Append(uint32_t pc, uint64_t offset) -> void
    {
        // Addresses cannot go backwards.
        if (pc < lastPc_ || offset < lastOffset_)
        {
            throw std::runtime_error("Input value out of range");
        }

        uint32_t pcOfs = pc - lastPc_;
        uint64_t nativeOfs = offset - lastOffset_;

        // Relative values cannot exceed 255.
        if (pcOfs > 255 || nativeOfs > 255)
        {
            throw std::runtime_error("Relative value out of range");
        }

        lastPc_ = pc;
        lastOffset_ = offset;

        entries_.emplace_back(static_cast<uint8_t>(pcOfs), static_cast<uint8_t>(nativeOfs));

        std::cout << std::format("vm address {:2} is native offset 0x{:04x} in offset map\n", pc, offset);
    }

    auto Find(uint32_t pc) const -> std::optional<uint64_t>
    {
        // Bail early if pc is out of bounds.
        if (pc < startPc_ || pc > lastPc_)
        {
            return {};
        }

        uint32_t pcAcc = startPc_;
        uint32_t offsetAcc = 0;
        for (auto entry : entries_)
        {
            pcAcc += entry.pc;
            offsetAcc += entry.offset;
            if (pcAcc == pc)
            {
                return offsetAcc; // Return the offset as we found a mapping.
            }
            else if (pcAcc > pc)
            {
                return {}; // We've overshot, so there's no mapping.
            }
        }
        return {};
    }
};

struct CompiledFunction
{
    CpuFunc baseAddress; // The compiled output. Effectively the base address of the function.
    OffsetMap offsets;   // A mapping from VM address to offset relative to base address.
};

class CompiledFunctionTable
{
    std::vector<CompiledFunction> compiledFunctions_;

public:
    auto Add(CompiledFunction func) -> void { compiledFunctions_.push_back(func); }

    auto Find(uint32_t vmAddr) -> CpuFunc
    {
        for (const auto& [baseAddress, offsets] : compiledFunctions_)
        {
            if (auto offset = offsets.Find(vmAddr))
            {
                const auto lookedUpAddr = asmjit::ptr_as_func<CpuFunc>(reinterpret_cast<std::byte*>(baseAddress) + offset.value());
                return lookedUpAddr;
            }
        }
        return nullptr;
    }
};

// using AddressMap = std::unordered_map<uint32_t, CpuFunc>;

class AddressMap
{
    std::vector<std::pair<uint32_t, CpuFunc>> map_;

public:
    CpuFunc& operator[](uint32_t vmAddr)
    {
        if (auto it = std::ranges::find_if(map_, [this, vmAddr](const auto& e) { return e.first == vmAddr; }); it != map_.end())
        {
            return it->second;
        }
        map_.emplace_back(vmAddr, nullptr);
        return map_.back().second;
    }

    size_t size() const { return map_.size(); }
};

class DemoJit
{
    // Runtime designed for JIT - it holds relocated functions and controls their lifetime.
    asmjit::JitRuntime runtime_;

    // Logs errors to stdout.
    asmjit::FileLogger logger_; // = asmjit::FileLogger(stdout);

    // Holds code and relocation information during code generation.
    asmjit::CodeHolder code_;

    // An assembler that can emit code to the code holder.
    asmjit::x86::Assembler a_;

    // A map from VM addresses to the corresponding generated code.
    AddressMap addressMap_;

    // Pending offsets, yet to be resolved to addresses.
    using OffsetPair = std::pair<uint32_t, asmjit::Label>;
    std::vector<OffsetPair> pendingOffsets_;

    // A table of compiled functions.
    CompiledFunctionTable compiledFunctions_{};

    uint32_t startPc_{};
    uint32_t pc_{};

public:
    DemoJit()
    {
        // Initialize the code holder so that it can be used.
        code_.init(runtime_.environment(), runtime_.cpuFeatures());
        code_.setLogger(&logger_);
        code_.attach(&a_);
    }

    auto SetPc(uint32_t pc) -> void
    {
        startPc_ = pc;
        pc_ = pc;
    }

    // If a label exists for a given offset then return it, otherwise create a new pending offset and return its label.
    auto FindOrCreateLabel(uint32_t offset) -> asmjit::Label
    {
        if (auto it = std::ranges::find_if(pendingOffsets_, [dst = offset](const auto& p) -> bool { return p.first == dst; }); it != pendingOffsets_.end())
        {
            return it->second;
        }
        auto label = a_.newLabel();
        pendingOffsets_.emplace_back(offset, label);
        return label;
    }

    // Adds an offset at pc, increment pc and return its old value.
    auto AddOffset() -> uint32_t
    {
        auto label = FindOrCreateLabel(pc_);
        a_.bind(label);
        const auto oldPc = pc_;
        pc_ += 1;
        return oldPc;
    };

    // Returns the label of the most recently added offset.
    auto LastLabel() -> asmjit::Label { return pendingOffsets_.back().second; }

    // Resolves a VM address into a native address.
    auto Resolve(uint32_t vmAddr) -> CpuFunc
    {
        std::cout << std::format("Resolving function at pc = {:2}: ", vmAddr);
        // Look up the function using the address map.
        if (auto result = addressMap_[vmAddr])
        {
            std::cout << std::format("0x{:016x} from address map - ", reinterpret_cast<uintptr_t>(result));
            std::cout << std::format(" address map contains {} items\n", addressMap_.size());
            return result;
        }

        // Look up the function using the function table.
        if (auto lookedUpAddr = compiledFunctions_.Find(vmAddr))
        {
            std::cout << std::format("0x{:016x} from compiled function table - ", reinterpret_cast<uintptr_t>(lookedUpAddr));
            addressMap_[vmAddr] = lookedUpAddr;
            std::cout << std::format(" address map contains {} items\n", addressMap_.size());
            return lookedUpAddr;
        }

        std::cout << "unknown\n";

        return nullptr;
    }

    auto Compile() -> CpuFunc
    {
        // Partition the pending offsets into those whose labels are bound and those whose labels are not bound.
        const auto unboundOffsets = std::ranges::stable_partition(pendingOffsets_, [this](auto it) {
            const auto& label = it.second;
            return code_.isLabelBound(label);
        });
        const auto boundOffsets = std::ranges::subrange(std::ranges::begin(pendingOffsets_), std::ranges::begin(unboundOffsets));

        // For each unbound label, bind it to a shim that returns to the execution environment via SetNextAndReturn()
        // so that the execution environment can resolve the address.
        for (auto [nextPc, label] : unboundOffsets)
        {
            a_.bind(label);
            SetNextPcAndReturn(nextPc);
        }

        // The assembler is no longer needed from here, so detach it from the code holder.
        code_.detach(&a_);

        // Copy and relocate the generated code from the code holder to the JitRuntime.
        CpuFunc generatedFunc{};
        if (asmjit::Error err = runtime_.add(&generatedFunc, &code_))
        {
            std::cerr << "AsmJit failed: " << asmjit::DebugUtils::errorAsString(err) << '\n';
            // TODO: error handling.
        }

        // Fix up those offsets so that we have a direct mapping from VM addresses to native addresses.
        const auto baseAddress = code_.baseAddress();
        std::cout << std::format("Base address of compiled code: 0x{:08x}\n", baseAddress);
        OffsetMap offsetMap_{startPc_};
        for (auto [vmAddr, label] : boundOffsets)
        {
            auto offset = code_.labelOffset(label);
            offsetMap_.Append(vmAddr, offset);
        }
        compiledFunctions_.Add(CompiledFunction{.baseAddress = generatedFunc, .offsets = offsetMap_});
        addressMap_[startPc_] = generatedFunc;

        // Reset so that we're ready for the next round of compilation.
        // TODO: there's probably a nicer way of doing this.
        pendingOffsets_.clear();
        code_.reset();
        code_.init(runtime_.environment(), runtime_.cpuFeatures());
        code_.setLogger(&logger_);
        code_.attach(&a_);

        // Usual caveats about lifetimes.
        return generatedFunc;
    }

    // Returns from JITted code to the execution environment setting the return value to the address of the compiled
    // code to call next.
    auto ReturnWithAddress(asmjit::Label next) -> void
    {
        a_.lea(asmjit::x86::rax, asmjit::x86::ptr(next));
        a_.ret();
    }

    // Returns from JITted code to the execution environment.
    auto ReturnWithNull() -> void
    {
        a_.mov(asmjit::x86::rax, 0);
        a_.ret();
    }

    // Sets next pc from EAX and returns from JITted code to the execution environment.
    auto SetNextPcAndReturn() -> void
    {
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        ReturnWithNull();
    }

    // Sets next pc to the given value.
    auto SetNextPc(uint32_t nextPc) -> void
    {
        a_.mov(asmjit::x86::eax, nextPc);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
    }

    // Sets next pc to the given value and returns from JITted code to the execution environment.
    auto SetNextPcAndReturn(uint32_t nextPc) -> void
    {
        SetNextPc(nextPc);
        ReturnWithNull();
    }

    // Decrements `ticks` and returns to the execution environment if it goes negative.
    auto CountZero(uint32_t pc, asmjit::Label myself)
    {
        const asmjit::Label doNotReturn = a_.newLabel();
        a_.dec(ARG1);
        a_.jge(doNotReturn);

        SetNextPc(pc);
        ReturnWithAddress(myself);

        a_.bind(doNotReturn);
    }

    // Branches relative to pc.
    auto Branch(uint32_t pc, int32_t imm)
    {
        auto label = FindOrCreateLabel(pc + imm);
        a_.jmp(label);
    }

    // Signals a trap on the CPU.
    auto EmitTrap(Trap trap) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addr = TrapOfs();
        a_.mov(asmjit::x86::eax, static_cast<uint32_t>(trap));
        a_.mov(addr, asmjit::x86::eax);
        ReturnWithNull();

        return pc;
    }

    // Adds two registers and stores the result in a third.
    auto EmitAdd(Reg rd, Reg rs1, Reg rs2) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);
        const auto addrRd = XregOfs(rd);

        // Only emit code if we're not writing to x0, as that's always zero.
        if (rd != 0)
        {
            a_.mov(asmjit::x86::eax, addrRs1);
            a_.add(asmjit::x86::eax, addrRs2);
            a_.mov(addrRd, asmjit::x86::eax);
        }

        return pc;
    }

    // Adds a register and an immediate value and stores the result in a destination register.
    auto EmitAddi(Reg rd, Reg rs1, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRd = XregOfs(rd);

        // Only emit code if we're not writing to rd, as that's always zero.
        if (rd != 0)
        {
            // Add if rs1 is not x0, otherwise just move.
            if (rs1 != 0)
            {
                a_.mov(asmjit::x86::eax, addrRs1);
                a_.add(asmjit::x86::eax, imm);
            }
            else
            {
                a_.mov(asmjit::x86::eax, imm);
            }
            a_.mov(addrRd, asmjit::x86::eax);
        }

        return pc;
    }

    // Compares two registers and branches if they aren't equal.
    auto EmitBne(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        CountZero(pc, LastLabel());
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);

        // Only emit code if we're using different register numbers, because otherwise their contents are guaranteed to
        // be equal and we'll never take the branch.
        if (rs1 != rs2)
        {
            const asmjit::Label branchNotTaken = a_.newLabel();
            a_.mov(asmjit::x86::eax, addrRs1);

            a_.cmp(asmjit::x86::eax, addrRs2);
            a_.je(branchNotTaken);

            // We took the branch. nextPc <- pc + imm
            Branch(pc, imm);

            // We didn't take the branch. nextPc <- pc + 1
            a_.bind(branchNotTaken);
        }

        return pc;
    }

    // Compares two registers and branches if they are equal.
    auto EmitBeq(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        CountZero(pc, LastLabel());
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);
        const asmjit::Label branchNotTaken = a_.newLabel();

        // Only emit code if we're using different register numbers, because otherwise their contents are guaranteed to
        // be equal and we'll always take the branch.
        if (rs1 != rs2)
        {
            a_.mov(asmjit::x86::eax, addrRs1);
            a_.cmp(asmjit::x86::eax, addrRs2);
            a_.jne(branchNotTaken);
        }

        // We took the branch. nextPc <- pc + imm
        Branch(pc, imm);

        // We didn't take the branch. nextPc <- pc + 1
        if (rs1 != rs2)
        {
            a_.bind(branchNotTaken);
        }

        return pc;
    }

    // Do an indirect jmp ro [rs1 + rs2 + imm].
    auto EmitJmp(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);

        a_.mov(asmjit::x86::eax, addrRs1); // TODO: what if this was x0 ?
        a_.add(asmjit::x86::eax, addrRs2); // TODO: what if this was x0 ?
        a_.add(asmjit::x86::eax, imm);     // TODO: what if this was zero ?
        SetNextPcAndReturn();

        return pc;
    }

    auto JitBlock(const vm::Code& code, uint32_t pc) -> CpuFunc
    {
        std::cout << std::format("Compiling from pc = {}\n", pc);
        if (pc >= code.size())
        {
            return nullptr;
        }
        SetPc(pc);
        bool stopHere = false;
        for (auto it = code.cbegin() + pc; it != code.cend() && !stopHere; ++it)
        {
            const auto& ins = *it;
            switch (ins.op)
            {
            case vm::TRAP:
                EmitTrap(ins.trap);
                stopHere = true;
                break;
            case vm::ADD:
                EmitAdd(ins.reg3.r1, ins.reg3.r2, ins.reg3.r3);
                break;
            case vm::ADDI:
                EmitAddi(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                break;
            case vm::BNE:
                EmitBne(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                break;
            case vm::BEQ:
                EmitBeq(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                break;
            case vm::JMP:
                EmitJmp(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                stopHere = true;
                break;
            }
        }

        return stopHere ? Compile() : nullptr;
    }
};

class ExecutionEnvironment
{
    vm::Code code_{};
    std::vector<Cpu> cpus_{};
    Cpu* currentCpu_{};
    size_t currentIndex_{};
    DemoJit jit_{};

    auto Resolve(uint32_t pc) -> CpuFunc
    {
        // If the JIT already knows about the address then return it.
        CpuFunc func = jit_.Resolve(pc);
        if (func)
        {
            return func;
        }

        // If the JIT doesn't know about the address then attempt to JIT a basic block.
        func = jit_.JitBlock(code_, pc);

        // If that fails, then we have a bad address, so signal a trap on the CPU.
        if (func == nullptr)
        {
            currentCpu_->trap = Trap::BAD_ADDRESS;
        }

        return func;
    }

    void Populate(size_t n)
    {
        cpus_.reserve(n);
        for (size_t i = 0; i < n; i++)
        {
            Cpu cpu{};
            cpu.xreg[5] = 1 + static_cast<uint32_t>(i);
            cpus_.push_back(cpu);
        }
    }

public:
    static constexpr size_t NUM_CPUS = 4;

    ExecutionEnvironment(vm::Code& code) : code_{code} { Populate(NUM_CPUS); }
    ExecutionEnvironment(vm::Code&& code) : code_{std::move(code)} { Populate(NUM_CPUS); }

    auto SwitchContext() -> uint32_t
    {
        auto pc = currentCpu_->nextPc;
        std::cout << std::format("--- SwitchContext before: cpu={:04} pc={:04} trap={}\n", currentIndex_, pc, static_cast<uint32_t>(currentCpu_->trap));
        currentIndex_ = (currentIndex_ + 1) % cpus_.size();
        currentCpu_ = &cpus_[currentIndex_];
        pc = currentCpu_->nextPc;
        for (size_t count = 1; count < cpus_.size() && currentCpu_->trap != Trap::NONE; ++count)
        {
            currentIndex_ = (currentIndex_ + 1) % cpus_.size();
            currentCpu_ = &cpus_[currentIndex_];
            pc = currentCpu_->nextPc;
        }
        std::cout << std::format("--- SwitchContext  after: cpu={:04} pc={:04} trap={}\n", currentIndex_, pc, static_cast<uint32_t>(currentCpu_->trap));
        return pc;
    }

    auto Run() -> void
    {
        constexpr uint32_t ticks = 10;
        currentCpu_ = &cpus_[currentIndex_];

        // JIT the VM's code, one basic block (sort of) at a time, and run it.
        uint32_t pc = 0;
        CpuFunc func = nullptr;

        // Run while there's at least one CPU that can make progress.
        while (std::ranges::any_of(cpus_, [](const auto& cpu) { return cpu.trap == Trap::NONE; }))
        {
            std::cout << std::format("Running on cpu {} pc {}\n", currentIndex_, pc);
            for (func = Resolve(pc); func != nullptr && currentCpu_->trap == Trap::NONE; func = Resolve(pc))
            {
                // Run native code until we need to resolve an address.
                while (func != nullptr && currentCpu_->trap == Trap::NONE)
                {
                    // Call the native code and run it for `ticks` ticks.
                    func = reinterpret_cast<CpuFunc>(func(currentCpu_, ticks));
                    std::cout << std::format("pc {:2}, native func = 0x{:016x}\n", currentCpu_->nextPc, reinterpret_cast<uintptr_t>(func));
                    std::cout << std::format("cpu={:04} pc={:04} trap={}, x5={}\n", currentIndex_, pc, static_cast<uint32_t>(currentCpu_->trap),
                                             currentCpu_->xreg[5]);
                }

                // Context switch if the CPU isn't trapped.
                if (currentCpu_->trap == Trap::NONE)
                {
                    pc = SwitchContext();
                }
            }

            // The CPU is trapped, so handle the trap before context switching.
            std::cout << std::format("cpu {} execution ended with status: ", currentIndex_);
            switch (currentCpu_->trap)
            {
            case Trap::NONE:
                std::cout << "Success";
                break;
            case Trap::BAD_ADDRESS:
                std::cout << std::format("Bad Address: 0x{:04x}", pc);
                break;
            case Trap::HALT:
                std::cout << "Halt";
                break;
            }
            std::cout << '\n';
            pc = SwitchContext();
        }
    }
};

auto main() -> int
{
    try
    {
        constexpr uint32_t ITERATIONS = 10;

        // Assemble some VM instructions into `code`.
        vm::Code code;
        vm::Assembler a(code);

        // Basic block. Add a few things together and fall through.
        a.Add(1, 2, 3); // 0: add x1, x2, x3
        a.Add(0, 1, 1); // 1: add x0, x1, x1
        a.Add(2, 2, 2); // 2: add x2, x2, x2
        a.Beq(0, 0, 1); // 3: beq x0, x0, +1

        // Basic block. Set a counter.
        // a.Addi(5, 0, ITERATIONS); // 4: addi x5, x0, ITERATIONS
        a.Addi(0, 0, 0); // Filler so that I don't have to renumber.

        // Basic block. A loop that counts down from 10 to 0.
        a.Addi(1, 0, 10); // 5: addi x1, 0, 10
        a.Addi(1, 1, -1); // 6: addi x1, x1, -1
        a.Bne(1, 0, -1);  // 7: bne x1, x0, -1

        // Basic block. Set up a few registers.
        a.Addi(1, 0, 15360); //  8: addi x1, 0, 3c00h
        a.Addi(2, 0, 15361); //  9: addi x2, 0, 3c01h
        a.Addi(3, 0, 1023);  // 10: addi x3, 0, 3ffh
        a.Beq(0, 0, 1);      // 11: beq x0, x0, +1

        // Basic block. A loop that counts down from 10 to 0.
        a.Addi(1, 0, 10); // 12: addi x1, x0, 10
        a.Addi(1, 1, -1); // 13: addi x1, x1, -1
        a.Bne(1, 0, -1);  // 14: bne x1, x0, -1

        // Basic Block. Do an indirect jump to x1 + x0 + 0.
        a.Addi(1, 0, 17); // 15: addi x1, x0, 17
        a.Jmp(1, 0, 0);   // 16: jmp x1 + x0 + 0

        // Basic Block. Decrement counter in x5 and branch back if not zero. Note that the branch goes to a different
        // compiled function so we're not (yet) able to resolve it at compile time.
        a.Addi(5, 5, -1); // 17: addi x5, x5, -1
        a.Bne(5, 0, -13); // 18: bne x5, x0, -13

        // Basic block. Load a value into x1. Halt. Do not catch fire.
        a.Addi(1, 0, 1337); // 19: addi x1, x0, 1337
        a.Trap(Trap::HALT); // 20: trap halt

        // Run the code on the JIT.
        ExecutionEnvironment executionEnvironment(code);
        executionEnvironment.Run();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
