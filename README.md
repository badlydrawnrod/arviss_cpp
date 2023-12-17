# ARVISS (cpp)

A RISC-V instruction set simulator implemented using C++ concepts. Compare and
contrast with the trait-based [Rust implementation](https://github.dev/badlydrawnrod/).

## Building

Example:
```
$ cmake -B build
-- Configuring done
-- Generating done
-- Build files have been written to: /home/rod/projects/arviss_cpp/build

$ cmake --build build
[2/2] Linking CXX executable arviss_cpp
```
For more info see the [BUILDING](BUILDING.md) document.

### CMake Init

I am not a CMake expert. I find the whole thing frustrating, not least because CMake has been around
for so long that there's a lot of conflicting and/or out of date information out there. Fortunately,
some kind person created [CMake Init](https://github.com/friendlyanon/cmake-init) which collects a
bunch of CMake wisdom into a script that creates suitable scaffolding to build a CMake project.
Thank you **friendlyanon**, whoever you are!

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

## Running

Build the examples as described in [examples/README.md](examples/README.md). This will create some example
images in `examples/images/`.

Run the examples as follows:
```
$ build/apps/runner examples/images/hello.bin
Hello, world from C!
Hello, world from C!
...
Hello
```

# How it works

At a very high level, Arviss works by decoding RISC-V instructions for a given RISC-V CPU variant,
e.g., RV32IMF, and dispatching them to a handler that knows what to do with them.

Here's a possible main loop..

```cpp
    while (!cpu.IsTrapped())
    {
        auto ins = cpu.Fetch(); // Fetch.
        cpu.Dispatch(ins);      // Execute.
    }
```

Given a CPU named `cpu`, it fetches the instruction from memory using `cpu.Fetch()` and dispatches
it to an instruction handler using `cpu.Dispatch()`. It does this until the CPU encounters a trap
(e.g., a breakpoint, a system call, bad memory access). This is fairly straightfoward - many
instruction set simulators / emulators will have a main loop that breaks down into fetching an
instruction from memory, decoding it, then executing it.

Where things get a little more interesting is the implementation. In Arviss, nearly everything is
written in terms of C++ concepts, so the idea of what constitutes a CPU and what `Fetch()` and
`Dispatch()` do, is defined by a number of C++ concepts.

Let's walk through an example to see how concepts are used in Arviss.

Here's the same code again, but this time shown in the context of a function template.

```cpp
template<IsRv32iCpu Cpu>
auto Run(Cpu& cpu) -> void
{
    while (!cpu.IsTrapped())
    {
        auto ins = cpu.Fetch(); // Fetch.
        cpu.Dispatch(ins);      // Execute.
    }
}
```

In this example, the template parameter `Cpu` is given as `IsRv32iCpu`, which means that the type
parameter needs to satisfy the `IsRv32iCpu` concept.

Here's some code that calls it. For this code to compile, `BasicRv32iCpu` has to satisfy the
`IsRv32iCpu` concept.

```cpp
BasicRv32iCpu cpu{}; // Create a CPU.
Run(cpu);            // Run the CPU until it traps.
```

Let's approach this from the perspective of concepts. What does `IsRv32iCpu` mean?

Here's the definition.

```cpp
// T is a CPU capable of fetching, dispatching and handling RV32i instructions for an
// integer core.
template<typename T>
concept IsRv32iCpu = IsRv32iDispatcher<T> && IsIntegerCore<T>;
```

From this, we can see that for a type T to satisfy the `IsRv32iCpu` concept, T must satisfy two more
concepts, namely `IsRv32iDispatcher` and `IsIntegerCore`.

Let's explore `IsIntegerCore` further.

```cpp
// T has all the pieces of an integer core.
template<typename T>
concept IsIntegerCore = HasTraps<T> // It has traps.
        && HasXRegisters<T>         // It has integer registers.
        && HasFetch<T>              // It has a fetch cycle implementation.
        && HasMemory<T>;            // It has memory.
```

Based on this, we can see that for a type T to satisfy the `IsIntegerCore` concept means that T
must support traps, have integer registers, an implementation of an instruction fetch cycle, and it
must have memory.

Here's an implementation of an integer core.

```cpp
// A mixin implementation of an integer core. BYO memory.
// Satisfies: IsIntegerCore (HasTraps, HasXRegisters, HasFetch, HasMemory)
template<HasMemory Mem>
struct IntegerCore : public TrapHandler, public XRegisters, public Fetcher<Mem>
{
};
```

Even this is implemented in terms of concepts, but at least we're beginning to see some concrete
types, namely `TrapHandler` and `XRegisters`.

Let's look at each of `XRegisters`, `TrapHandler` and `Fetcher<Mem>` in turn.

Here's `XRegisters`.

```cpp
// A mixin implementation of RV32i's integer registers.
// Satisfies: HasXRegisters
class XRegisters
{
    std::array<u32, 32> xreg_{};

public:
    auto Rx(Reg rs) -> u32 { return xreg_[rs]; }

    auto Wx(Reg rd, u32 val) -> void
    {
        xreg_[rd] = val;
        xreg_[0] = 0;
    }
};
```

A RISC-V CPU has a number of integer registers named `x0` - `x31`. All of them are read-write, with
the notable exception of `x0` whose content is always zero.

The implementation of `XRegisters` has two member functions:
- `Rx()` returns the contents of a given register.
- `Wx()` sets the contents of a given register.

`XRegisters` satisfies the `HasXRegisters` concept. Here's how `HasXRegisters` is specified.

```cpp
// T supports reading from and writing to integer registers.
template<typename T>
concept HasXRegisters = requires(T t, u32 result) {
    result = t.Rx(Reg{}); // Reads from an integer register.
    t.Wx(Reg{}, u32{});   // Writes to an integer register.
};
```

There's no much to it. For a type T to satisfy `HasXRegisters` the two statements in the `requires`
expression need to be well formed for T. In other words, given an instance of `XRegisters`, the
following would compile.

```cpp
XRegisters t;
u32 result;
result = t.Rx(Reg{}); // Reads from an integer register.
t.Wx(Reg{}, u32{});   // Writes to an integer register.
```

Any type T for which those statements are well formed satisfies the `HasXRegisters` concept.

Here's another implementation, `CArrayXRegisters`. It also satisifies the `HasXRegisters` concept.

```cpp
// Satisfies: HasXRegisters
class CArrayXRegisters
{
    u32 rs[32];

public:
    u32 Rx(Reg rs)
    {
        return rs[rs];
    }

    void Wx(Reg rd, u32 val)
    {
        rs[rd] = val;
        rs[0] = 0;
    }
};
```

Here's the implementation of `TrapHandler`.

```cpp
// A mixin implementation of a trap handler.
// Satisfies: HasTraps.
class TrapHandler
{
    std::optional<TrapState> trap_{};

public:
    auto IsTrapped() const -> bool { return trap_.has_value(); }
    auto TrapCause() const -> std::optional<TrapState> { return trap_; }
    auto RaiseTrap(TrapType type, u32 context = 0) { trap_ = {.type_ = type, .context_ = context}; }
    auto ClearTraps() { trap_ = {}; }
};
```

The implementation satisfies `HasTraps`, specified below.

```cpp
// T has traps.
template<typename T>
concept HasTraps = requires(T t, TrapType type, u32 context, bool b, std::optional<TrapState> c) {
    b = t.IsTrapped();          // Returns true if a trap is currently active.
    c = t.TrapCause();          // Returns the cause of the trap.
    t.RaiseTrap(type);          // It can raise a trap of the given type.
    t.RaiseTrap(type, context); // It can raise a trap of the given type with the given context.
    t.ClearTraps();             // It can clear traps.
};
```

Finally, here's the implementation of `Fetcher<Mem>`.

```cpp
// A mixin implementation of the fetch cycle for the given memory implementation. BYO memory.
// Satisfies: HasFetch, HasMemory
template<HasMemory Mem>
class Fetcher : public Mem
{
    Address pc_{};
    Address nextPc_{};

public:
    auto Pc() -> Address const { return pc_; }

    auto Transfer() -> Address
    {
        pc_ = nextPc_;
        return pc_;
    }

    auto Fetch() -> u32
    {
        auto pc = Transfer();
        auto ins = Fetch32(pc);
        SetNextPc(pc + 4);
        return ins;
    }

    auto SetNextPc(Address address) -> void { nextPc_ = address; }

    auto Fetch32(Address address) -> u32
    {
        auto& mem_ = static_cast<Mem&>(*this);
        return mem_.Read32(address);
    }
};
```

The implementation satisfies both `HasFetch` and `HasMemory`, because it directly implements the
requirements of `HasFetch`, and it implements the requirements of `HasMemory` by inheriting from
`Mem`.

Here's the concept `HasFetch`.

```cpp
// T implements the fetch cycle.
template<typename T>
concept HasFetch = requires(T t, Address a, u32 w) {
    a = t.Pc();               // Returns the contents of the program counter.
    a = t.Transfer();         // Transfers nextPc to the program counter and returns it.
    w = t.Fetch();            // Transfer, Fetch32(pc), SetNextPc, return instruction.
    t.SetNextPc(Address{});   // Sets nextPc.
    w = t.Fetch32(Address{}); // Returns the instruction at the given address.
};
```

And here's the concept `HasMemory`.

```cpp
// T supports reading from and writing to memory.
template<typename T>
concept HasMemory = requires(T t, u8 b, u16 h, u32 w) {
    b = t.Read8(Address{});      // Reads a byte from an address.
    h = t.Read16(Address{});     // Reads a halfword from an address.
    w = t.Read32(Address{});     // Reads a word from an address.
    t.Write8(Address{}, u8{});   // Writes a byte to an address.
    t.Write16(Address{}, u16{}); // Writes a halfword to an address.
    t.Write32(Address{}, u32{}); // Writes a word to an address.
};
```

In other words, `Fetcher` is written in terms of `Mem` where `Mem` satisifies the concept `HasMemory`.

Returning to `IsIntegerCore`, we can now see that for T to satisfy `IsIntegerCore` it needs to
satisfy all of the listed concepts.

```cpp
// T has all the pieces of an integer core.
template<typename T>
concept IsIntegerCore = HasTraps<T> // It has traps.
        && HasXRegisters<T>         // It has integer registers.
        && HasFetch<T>              // It has a fetch cycle implementation.
        && HasMemory<T>;            // It has memory.
```

Similarly, `IsRv32iCpu` must satisfy both `IsRv32iDispatcher` and `IsIntegerCore`.

```cpp
// T is a CPU capable of fetching, dispatching and handling RV32i instructions for an
// integer core.
template<typename T>
concept IsRv32iCpu = IsRv32iDispatcher<T> && IsIntegerCore<T>;
```

We've examined `IsIntegerCore`, but what does `IsRv32iDispatcher` mean?

```cpp
// T is an instruction dispatcher for Rv32i instruction handlers.
template<typename T>
concept IsRv32iDispatcher = IsDispatcher<T> && IsRv32iInstructionHandler<T>;
```

It turns out that it has to satisfy both `IsDispatcher` and `IsRv32iInstructionHandler`.

The first of these is simple to define. Here's the concept `IsDispatcher`.

```cpp
// T is an instruction dispatcher.
template<typename T>
concept IsDispatcher = requires(T t) {
    {
        t.Dispatch(u32{})
    } -> std::same_as<typename T::Item>;
};
```

This says that for a given type T, the statement `t.Dispatch(u32{})` must be well formed and its
return type must be the same as a type `T::Item`. So whatever type T is, it must have a `Dispatch()`
member function whose return value is of type `T::Item`. Therefore, T must define an `Item` type.


The concept `IsRv32iInstructionHandler` is defined in terms of two concepts. For a type T to satisfy
`IsRv32iInstructionHandler` it must satisfy either `IsNonVoidRv32iInstructionHandler` *OR*
`IsVoidRv32iInstructionHandler`.

```cpp
// T is an instruction handler for Rv32i instructions.
template<typename T>
concept IsRv32iInstructionHandler = IsNonVoidRv32iInstructionHandler<T>
                                 || IsVoidRv32iInstructionHandler<T>;
```

Here's the latter. For a type T to satisfy `IsVoidRv32iInstructionHandler`, it must have
implementations for all of the listed member functions, and T's `Item` type must be `void`.

```cpp
// T is an instruction handler for Rv32i instructions whose member functions do not return a value.
template<typename T>
concept IsVoidRv32iInstructionHandler = std::same_as<void, typename T::Item> && requires(T self) {
    self.Ecall();
    self.Ebreak();
    self.Add(Reg{}, Reg{}, Reg{});
    self.Sub(Reg{}, Reg{}, Reg{});
    self.Sll(Reg{}, Reg{}, Reg{});
    self.Slt(Reg{}, Reg{}, Reg{});
    self.Sltu(Reg{}, Reg{}, Reg{});
    self.Xor(Reg{}, Reg{}, Reg{});
    self.Srl(Reg{}, Reg{}, Reg{});
    self.Sra(Reg{}, Reg{}, Reg{});
    self.Or(Reg{}, Reg{}, Reg{});
    self.And(Reg{}, Reg{}, Reg{});
    self.Slli(Reg{}, Reg{}, u32{});
    self.Srli(Reg{}, Reg{}, u32{});
    self.Srai(Reg{}, Reg{}, u32{});
    self.Beq(Reg{}, Reg{}, u32{});
    self.Bne(Reg{}, Reg{}, u32{});
    self.Blt(Reg{}, Reg{}, u32{});
    self.Bge(Reg{}, Reg{}, u32{});
    self.Bltu(Reg{}, Reg{}, u32{});
    self.Bgeu(Reg{}, Reg{}, u32{});
    self.Jalr(Reg{}, Reg{}, u32{});
    self.Addi(Reg{}, Reg{}, u32{});
    self.Slti(Reg{}, Reg{}, u32{});
    self.Sltiu(Reg{}, Reg{}, u32{});
    self.Xori(Reg{}, Reg{}, u32{});
    self.Ori(Reg{}, Reg{}, u32{});
    self.Andi(Reg{}, Reg{}, u32{});
    self.Lb(Reg{}, Reg{}, u32{});
    self.Lh(Reg{}, Reg{}, u32{});
    self.Lw(Reg{}, Reg{}, u32{});
    self.Lbu(Reg{}, Reg{}, u32{});
    self.Lhu(Reg{}, Reg{}, u32{});
    self.Sb(Reg{}, Reg{}, u32{});
    self.Sh(Reg{}, Reg{}, u32{});
    self.Sw(Reg{}, Reg{}, u32{});
    self.Fence(u32{}, Reg{}, Reg{});
    self.Jal(Reg{}, u32{});
    self.Lui(Reg{}, u32{});
    self.Auipc(Reg{}, u32{});
};
```

Here's a dispatcher that satisfies `IsRv32iDispatcher`.

```cpp
// A dispatcher for RV32I instructions. BYO handler.
template<typename Handler>
    requires IsRv32iInstructionHandler<Handler>
struct Rv32iDispatcher : public Handler
{
    using Item = Handler::Item;

    // Decodes the input word to an RV32I instruction and dispatches it to a handler.
    auto Dispatch(u32 code) -> Item
    {
        Handler& self = static_cast<Handler&>(*this);
        Instruction c(code);

        switch (code) {
            case 0x00000073: return self.Ecall();
            case 0x00100073: return self.Ebreak();
        }
        switch (code & 0xfe00707f) {
            case 0x00000033: return self.Add(c.Rd(), c.Rs1(), c.Rs2());
            case 0x40000033: return self.Sub(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00001033: return self.Sll(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00002033: return self.Slt(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00003033: return self.Sltu(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00004033: return self.Xor(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00005033: return self.Srl(c.Rd(), c.Rs1(), c.Rs2());
            case 0x40005033: return self.Sra(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00006033: return self.Or(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00007033: return self.And(c.Rd(), c.Rs1(), c.Rs2());
            case 0x00001013: return self.Slli(c.Rd(), c.Rs1(), c.Shamtw());
            case 0x00005013: return self.Srli(c.Rd(), c.Rs1(), c.Shamtw());
            case 0x40005013: return self.Srai(c.Rd(), c.Rs1(), c.Shamtw());
        }
        switch (code & 0x0000707f) {
            case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.Bimmediate());
            case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.Iimmediate());
            case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.Simmediate());
            case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.Simmediate());
            case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.Simmediate());
            case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
        }
        switch (code & 0x0000007f) {
            case 0x0000006f: return self.Jal(c.Rd(), c.Jimmediate());
            case 0x00000037: return self.Lui(c.Rd(), c.Uimmediate());
            case 0x00000017: return self.Auipc(c.Rd(), c.Uimmediate());
        }
        return self.Illegal(code);
    }
};
```

It implements `Dispatch()` itself, but it inherits the implementation of
`IsRv32iInstructionHandler`, and it defines its `Item` type in terms of the one
inherited from the instruction handler.

It doesn't matter what the instruction handler looks like, or what it does. It just needs to satisfy
the concept `IsRv32iInstructionHandler`.

This is very powerful - it means that the instruction handler could execute instructions, or it
could write them to stdout, or both, or it could do something completely different that hasn't been
thought of yet. None of that matters to the dispatcher, as it is effectively an instruction decode
for RV32i instructions that doesn't need to know anything about the implementation of the handler
other than the fact that the handler satisfies the `IsRv32iInstructionHandler` concept.

Here's a fragment of an instruction handler that satisfies `IsRv32iInstructionHandler`. This
particular instruction handler operates on any type T that satisfies the `IsIntegerCore` concept.
The example has been shortened to prevent it from being too verbose, but it clearly shows
implementations for three of the member functions that satisfy `IsRv32iInstructionHandler`.
 
 ```cpp
// An Rv32i instruction handler that executes instructions on an integer core. BYO core.
template<IsIntegerCore T>
class Rv32iIntegerCoreExecutor : public T
{
    auto Self() -> T& { return static_cast<T&>(*this); }

    // ...

public:
    using Item = void;

    // ...

    // Immediate shift instructions.

    auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) << shamt);
    }

    auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) >> shamt);
    }

    auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        auto xreg_rs = static_cast<i32>(self.Rx(rs1));
        self.Wx(rd, static_cast<u32>(xreg_rs >> static_cast<i32>(shamt)));
    }

    // ...
};
```

To illustrate the power of C++ concepts, here's a code fragment for another type that also satisfies
`IsRv32iInstructionHandler`, showing implementations for the same member functions.

```cpp
// An instruction handler for a disassembler for Rv32i instructions.
struct Rv32iDisassemblingHandler
{
    using Item = std::string;

    // ...

    // Immediate shift instructions.

    auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        return std::format("slli\t{}, {}, {}", Abi(rd), Abi(rs1), shamt);
    }
    
    auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        return std::format("srli\t{}, {}, {}", Abi(rd), Abi(rs1), shamt);
    }
    
    auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        return std::format("srai\t{}, {}, {}", Abi(rd), Abi(rs1), shamt);
    }

    // ...
};
```

In this case, the `Item` type is `std::string` rather than `void`. Nonetheless,
`Rv32iDisassemblingHandler` satisfies the concept `IsRv32iInstructionHandler` and can therefore be
used with a dispatcher such as `Rv32iDispatcher` shown earlier to implement a disassembler. The
dispatcher doesn't need to know that it's dealing with a disassembler - it only needs to know that
it's dealing with a type that satisfies `IsRv32iInstructionHandler`.

## Summary

In summary, this C++ implementation of Arviss is written in terms of high level C++ concepts such
as `IsIntegerCore` and `IsRv32iDispatcher`, which in turn are built in terms of more fine-grained
C++ concepts such as `IsRv32iInstructionHandler`, `HasMemory` and `HasXRegisters`.

This enables an enormous amount of flexibility so that someone implementing a system that needs a
virtual RISC-V CPU can choose how to implement each element of that system, and can substitute each
defined element (concept) in a myriad of ways, as long as the relevant requirements for each concept
are satisfied.
