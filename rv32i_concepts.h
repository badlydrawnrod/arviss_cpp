#pragma once

#include "core_concepts.h"
#include "types.h"

#include <concepts>
#include <string>

// TYPES: RV32i concepts.

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

// T is an instruction handler for Rv32i instructions whose member functions return a value.
template<typename T>
concept IsNonVoidRv32iInstructionHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
    item = self.Ecall();
    item = self.Ebreak();
    item = self.Add(Reg{}, Reg{}, Reg{});
    item = self.Sub(Reg{}, Reg{}, Reg{});
    item = self.Sll(Reg{}, Reg{}, Reg{});
    item = self.Slt(Reg{}, Reg{}, Reg{});
    item = self.Sltu(Reg{}, Reg{}, Reg{});
    item = self.Xor(Reg{}, Reg{}, Reg{});
    item = self.Srl(Reg{}, Reg{}, Reg{});
    item = self.Sra(Reg{}, Reg{}, Reg{});
    item = self.Or(Reg{}, Reg{}, Reg{});
    item = self.And(Reg{}, Reg{}, Reg{});
    item = self.Slli(Reg{}, Reg{}, u32{});
    item = self.Srli(Reg{}, Reg{}, u32{});
    item = self.Srai(Reg{}, Reg{}, u32{});
    item = self.Beq(Reg{}, Reg{}, u32{});
    item = self.Bne(Reg{}, Reg{}, u32{});
    item = self.Blt(Reg{}, Reg{}, u32{});
    item = self.Bge(Reg{}, Reg{}, u32{});
    item = self.Bltu(Reg{}, Reg{}, u32{});
    item = self.Bgeu(Reg{}, Reg{}, u32{});
    item = self.Jalr(Reg{}, Reg{}, u32{});
    item = self.Addi(Reg{}, Reg{}, u32{});
    item = self.Slti(Reg{}, Reg{}, u32{});
    item = self.Sltiu(Reg{}, Reg{}, u32{});
    item = self.Xori(Reg{}, Reg{}, u32{});
    item = self.Ori(Reg{}, Reg{}, u32{});
    item = self.Andi(Reg{}, Reg{}, u32{});
    item = self.Lb(Reg{}, Reg{}, u32{});
    item = self.Lh(Reg{}, Reg{}, u32{});
    item = self.Lw(Reg{}, Reg{}, u32{});
    item = self.Lbu(Reg{}, Reg{}, u32{});
    item = self.Lhu(Reg{}, Reg{}, u32{});
    item = self.Sb(Reg{}, Reg{}, u32{});
    item = self.Sh(Reg{}, Reg{}, u32{});
    item = self.Sw(Reg{}, Reg{}, u32{});
    item = self.Fence(u32{}, Reg{}, Reg{});
    item = self.Jal(Reg{}, u32{});
    item = self.Lui(Reg{}, u32{});
    item = self.Auipc(Reg{}, u32{});
};

// T is an instruction handler for Rv32i instructions.
template<typename T>
concept IsRv32iInstructionHandler = IsNonVoidRv32iInstructionHandler<T> || IsVoidRv32iInstructionHandler<T>;

// T is an instruction dispatcher for Rv32i instruction handlers.
template<typename T>
concept IsRv32iDispatcher = IsDispatcher<T> && IsRv32iInstructionHandler<T>;

// T is a VM capable of fetching, dispatching and handling RV32i instructions for an integer CPU.
template<typename T>
concept IsRv32iVm = IsRv32iDispatcher<T> && IsIntCpu<T>;

// T is a dispatcher for a tracing handler for Rv32i instructions.
template<typename T>
concept IsRv32iTrace = IsRv32iDispatcher<T> && std::same_as<std::string, typename T::Item>;
