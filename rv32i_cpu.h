#pragma once

#include "concepts.h"
#include "integer_cpu.h"
#include "rv32i_dispatcher.h"
#include "rv32i_instruction_handler.h"

// An RV32i CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher. RV32i specific.
template<IsIntCpu T>
using RV32iCpu = MRv32iDispatcher<MRv32iHandler<T>>;

// A mixin RV32i CPU. BYO memory. RV32i specific.
// A specialization of RV32iCpu for a MIntCpu with an unspecified memory implementation.
template<HasMemory Mem>
using AbstractRV32iCpu = RV32iCpu<MIntCpu<Mem>>;
