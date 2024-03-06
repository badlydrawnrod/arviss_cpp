#pragma once

#include "arviss/arviss.h"

#include <concepts>
#include <string>

namespace arviss
{
    namespace impl
    {
        // T is an instruction handler for Rv32i instructions whose member functions do not return a value.
        template<typename T>
        concept IsVoidRv32iHandler = std::same_as<void, typename T::Item> && requires(T self) {
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
        concept IsNonVoidRv32iHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
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
    } // namespace impl

    // T is an instruction handler for Rv32i instructions.
    template<typename T>
    concept IsRv32iHandler = impl::IsNonVoidRv32iHandler<T> || impl::IsVoidRv32iHandler<T>;

    // T is an instruction dispatcher for Rv32i instruction handlers.
    template<typename T>
    concept IsRv32iDispatcher = IsDispatcher<T> && IsRv32iHandler<T>;

    // T is a dispatcher CPU capable of fetching, dispatching and handling RV32i instructions for an integer core.
    template<typename T>
    concept IsRv32iCpu = IsRv32iDispatcher<T> && IsIntegerCore<T>;

    // T is a dispatcher for a tracing handler for Rv32i instructions.
    template<typename T>
    concept IsRv32iTrace = IsRv32iDispatcher<T> && std::same_as<std::string, typename T::Item>;

    namespace impl
    {
        // T is an instruction handler for Rv32m instructions whose member functions do not return a value.
        template<typename T>
        concept IsVoidRv32mHandler = std::same_as<void, typename T::Item> && requires(T self) {
            self.Mul(Reg{}, Reg{}, Reg{});
            self.Mulh(Reg{}, Reg{}, Reg{});
            self.Mulhsu(Reg{}, Reg{}, Reg{});
            self.Mulhu(Reg{}, Reg{}, Reg{});
            self.Div(Reg{}, Reg{}, Reg{});
            self.Divu(Reg{}, Reg{}, Reg{});
            self.Rem(Reg{}, Reg{}, Reg{});
            self.Remu(Reg{}, Reg{}, Reg{});
        };

        // T is an instruction handler for Rv32m instructions whose member functions return a value.
        template<typename T>
        concept IsNonVoidRv32mHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
            item = self.Mul(Reg{}, Reg{}, Reg{});
            item = self.Mulh(Reg{}, Reg{}, Reg{});
            item = self.Mulhsu(Reg{}, Reg{}, Reg{});
            item = self.Mulhu(Reg{}, Reg{}, Reg{});
            item = self.Div(Reg{}, Reg{}, Reg{});
            item = self.Divu(Reg{}, Reg{}, Reg{});
            item = self.Rem(Reg{}, Reg{}, Reg{});
            item = self.Remu(Reg{}, Reg{}, Reg{});
        };
    } // namespace impl

    // T is an instruction handler for Rv32m instructions.
    template<typename T>
    concept IsRv32mHandler = impl::IsNonVoidRv32mHandler<T> || impl::IsVoidRv32mHandler<T>;

    // T is an instruction dispatcher for Rv32m instruction handlers.
    template<typename T>
    concept IsRv32mDispatcher = IsDispatcher<T> && IsRv32mHandler<T>;

    // T is an instruction handler for RV32im instructions.
    template<typename T>
    concept IsRv32imHandler = IsRv32iHandler<T> && IsRv32mHandler<T>;

    // T is an instruction dispatcher for Rv32im instruction handlers.
    template<typename T>
    concept IsRv32imDispatcher = IsDispatcher<T> && IsRv32imHandler<T>;

    // T is a CPU capable of fetching, dispatching and handling RV32im instructions for an integer core.
    template<typename T>
    concept IsRv32imCpu = IsRv32imDispatcher<T> && IsIntegerCore<T>;

    // T is a dispatcher for a tracing handler for Rv32im instructions.
    template<typename T>
    concept IsRv32imTrace = IsRv32imDispatcher<T> && std::same_as<std::string, typename T::Item>;

    namespace impl
    {
        // T is an instruction handler for Rv32c instructions whose member functions do not return a value.
        template<typename T>
        concept IsVoidRv32cHandler = std::same_as<void, typename T::Item> && requires(T self) {
            self.C_ebreak();
            self.C_jr(Reg{});
            self.C_jalr(Reg{});
            self.C_nop(u32{});
            self.C_addi16sp(Reg{});
            self.C_sub(Reg{}, Reg{});
            self.C_xor(Reg{}, Reg{});
            self.C_or(Reg{}, Reg{});
            self.C_and(Reg{}, Reg{});
            self.C_andi(Reg{}, u32{});
            self.C_srli(Reg{}, u32{});
            self.C_srai(Reg{}, u32{});
            self.C_mv(Reg{}, Reg{});
            self.C_add(Reg{}, Reg{});
            self.C_addi4spn(Reg{}, u32{});
            self.C_lw(Reg{}, Reg{}, u32{});
            self.C_sw(Reg{}, Reg{}, u32{});
            self.C_addi(Reg{}, u32{});
            self.C_li(Reg{}, u32{});
            self.C_lui(Reg{}, u32{});
            self.C_j(u32{});
            self.C_beqz(Reg{}, u32{});
            self.C_bnez(Reg{}, u32{});
            self.C_lwsp(Reg{}, u32{});
            self.C_swsp(Reg{}, u32{});
            self.C_jal(u32{});
            self.C_slli(Reg{}, u32{});
        };

        // T is an instruction handler for Rv32c instructions whose member functions return a value.
        template<typename T>
        concept IsNonVoidRv32cHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
            item = self.C_ebreak();
            item = self.C_jr(Reg{});
            item = self.C_jalr(Reg{});
            item = self.C_nop(u32{});
            item = self.C_addi16sp(Reg{});
            item = self.C_sub(Reg{}, Reg{});
            item = self.C_xor(Reg{}, Reg{});
            item = self.C_or(Reg{}, Reg{});
            item = self.C_and(Reg{}, Reg{});
            item = self.C_andi(Reg{}, u32{});
            item = self.C_srli(Reg{}, u32{});
            item = self.C_srai(Reg{}, u32{});
            item = self.C_mv(Reg{}, Reg{});
            item = self.C_add(Reg{}, Reg{});
            item = self.C_addi4spn(Reg{}, u32{});
            item = self.C_lw(Reg{}, Reg{}, u32{});
            item = self.C_sw(Reg{}, Reg{}, u32{});
            item = self.C_addi(Reg{}, u32{});
            item = self.C_li(Reg{}, u32{});
            item = self.C_lui(Reg{}, u32{});
            item = self.C_j(u32{});
            item = self.C_beqz(Reg{}, u32{});
            item = self.C_bnez(Reg{}, u32{});
            item = self.C_lwsp(Reg{}, u32{});
            item = self.C_swsp(Reg{}, u32{});
            item = self.C_jal(u32{});
            item = self.C_slli(Reg{}, u32{});
        };
    } // namespace impl

    // T is an instruction handler for Rv32c instructions.
    template<typename T>
    concept IsRv32cHandler = impl::IsNonVoidRv32cHandler<T> || impl::IsVoidRv32cHandler<T>;

    // T is an instruction dispatcher for Rv32c instruction handlers.
    template<typename T>
    concept IsRv32cDispatcher = IsDispatcher<T> && IsRv32cHandler<T>;

    // T is an instruction handler for RV32ic instructions.
    template<typename T>
    concept IsRv32icHandler = IsRv32iHandler<T> && IsRv32cHandler<T>;

    // T is an instruction dispatcher for Rv32ic instruction handlers.
    template<typename T>
    concept IsRv32icDispatcher = IsDispatcher<T> && IsRv32icHandler<T>;

    // T is a CPU capable of fetching, dispatching and handling RV32ic instructions for an integer core.
    template<typename T>
    concept IsRv32icCpu = IsRv32icDispatcher<T> && IsIntegerCore<T>;

    // T is a dispatcher for a tracing handler for Rv32ic instructions.
    template<typename T>
    concept IsRv32icTrace = IsRv32icDispatcher<T> && std::same_as<std::string, typename T::Item>;

    namespace impl
    {
        // T is an instruction handler for Rv32f instructions whose member functions do not return a value.
        template<typename T>
        concept IsVoidRv32fHandler = std::same_as<void, typename T::Item> && requires(T self) {
            self.Fmv_x_w(Reg{}, Reg{});
            self.Fclass_s(Reg{}, Reg{});
            self.Fmv_w_x(Reg{}, Reg{});
            self.Fsqrt_s(Reg{}, Reg{}, u32{});
            self.Fcvt_w_s(Reg{}, Reg{}, u32{});
            self.Fcvt_wu_s(Reg{}, Reg{}, u32{});
            self.Fcvt_s_w(Reg{}, Reg{}, u32{});
            self.Fcvt_s_wu(Reg{}, Reg{}, u32{});
            self.Fsgnj_s(Reg{}, Reg{}, Reg{});
            self.Fsgnjn_s(Reg{}, Reg{}, Reg{});
            self.Fsgnjx_s(Reg{}, Reg{}, Reg{});
            self.Fmin_s(Reg{}, Reg{}, Reg{});
            self.Fmax_s(Reg{}, Reg{}, Reg{});
            self.Fle_s(Reg{}, Reg{}, Reg{});
            self.Flt_s(Reg{}, Reg{}, Reg{});
            self.Feq_s(Reg{}, Reg{}, Reg{});
            self.Fadd_s(Reg{}, Reg{}, Reg{}, u32{});
            self.Fsub_s(Reg{}, Reg{}, Reg{}, u32{});
            self.Fmul_s(Reg{}, Reg{}, Reg{}, u32{});
            self.Fdiv_s(Reg{}, Reg{}, Reg{}, u32{});
            self.Flw(Reg{}, Reg{}, u32{});
            self.Fsw(Reg{}, Reg{}, u32{});
            self.Fmadd_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            self.Fmsub_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            self.Fnmsub_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            self.Fnmadd_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
        };

        // T is an instruction handler for Rv32f instructions whose member functions return a value.
        template<typename T>
        concept IsNonVoidRv32fHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
            item = self.Fmv_x_w(Reg{}, Reg{});
            item = self.Fclass_s(Reg{}, Reg{});
            item = self.Fmv_w_x(Reg{}, Reg{});
            item = self.Fsqrt_s(Reg{}, Reg{}, u32{});
            item = self.Fcvt_w_s(Reg{}, Reg{}, u32{});
            item = self.Fcvt_wu_s(Reg{}, Reg{}, u32{});
            item = self.Fcvt_s_w(Reg{}, Reg{}, u32{});
            item = self.Fcvt_s_wu(Reg{}, Reg{}, u32{});
            item = self.Fsgnj_s(Reg{}, Reg{}, Reg{});
            item = self.Fsgnjn_s(Reg{}, Reg{}, Reg{});
            item = self.Fsgnjx_s(Reg{}, Reg{}, Reg{});
            item = self.Fmin_s(Reg{}, Reg{}, Reg{});
            item = self.Fmax_s(Reg{}, Reg{}, Reg{});
            item = self.Fle_s(Reg{}, Reg{}, Reg{});
            item = self.Flt_s(Reg{}, Reg{}, Reg{});
            item = self.Feq_s(Reg{}, Reg{}, Reg{});
            item = self.Fadd_s(Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fsub_s(Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fmul_s(Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fdiv_s(Reg{}, Reg{}, Reg{}, u32{});
            item = self.Flw(Reg{}, Reg{}, u32{});
            item = self.Fsw(Reg{}, Reg{}, u32{});
            item = self.Fmadd_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fmsub_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fnmsub_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
            item = self.Fnmadd_s(Reg{}, Reg{}, Reg{}, Reg{}, u32{});
        };
    } // namespace impl

    // T is an instruction handler for Rv32f instructions.
    template<typename T>
    concept IsRv32fHandler = impl::IsVoidRv32fHandler<T> || impl::IsNonVoidRv32fHandler<T>;

    // T is an instruction dispatcher for Rv32f instruction handlers.
    template<typename T>
    concept IsRv32fDispatcher = IsDispatcher<T> && IsRv32fHandler<T>;

    // T is an instruction handler for RV32if instructions.
    template<typename T>
    concept IsRv32ifHandler = IsRv32iHandler<T> && IsRv32fHandler<T>;

    // T is an instruction handler for RV32imf instructions.
    template<typename T>
    concept IsRv32imfHandler = IsRv32iHandler<T> && IsRv32mHandler<T> && IsRv32fHandler<T>;

    // T is an instruction dispatcher for Rv32imf instruction handlers.
    template<typename T>
    concept IsRv32imfDispatcher = IsDispatcher<T> && IsRv32imfHandler<T>;

    // T is a CPU capable of fetching, dispatching and handling RV32imf instructions for a floating point core.
    template<typename T>
    concept IsRv32imfCpu = IsRv32imfDispatcher<T> && IsFloatCore<T>;

    // T is a dispatcher for a tracing handler for Rv32imf instructions.
    template<typename T>
    concept IsRv32imfTrace = IsRv32imfDispatcher<T> && std::same_as<std::string, typename T::Item>;

} // namespace arviss
