#pragma once

#include "core_concepts.h"
#include "core_mixins.h"
#include "rv32i_dispatcher.h"
#include "rv32i_instruction_handler.h"
#include "rv32ic_dispatcher.h"
#include "rv32im_dispatcher.h"

// An RV32i CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher.
template<IsIntegerCore T>
using Rv32iCpu = MRv32iDispatcher<MRv32iHandler<T>>;

// A mixin RV32i CPU. BYO memory.
// A specialization of Rv32iCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using Rv32iCpuMIntegerCore = Rv32iCpu<MIntegerCore<Mem>>;

// An RV32im CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher.
template<IsIntegerCore T>
using Rv32imCpu = MRv32imDispatcher<MRv32imHandler<T>>;

// A mixin RV32im CPU. BYO memory.
// A specialization of Rv32imCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using Rv32imCpuMIntegerCore = Rv32imCpu<MIntegerCore<Mem>>;

// An RV32ic CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher.
template<IsIntegerCore T>
using Rv32icCpu = MRv32icDispatcher<MRv32icHandler<T>>;

// A mixin RV32ic CPU. BYO memory.
// A specialization of Rv32icCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using Rv32icCpuMIntegerCore = Rv32icCpu<MIntegerCore<Mem>>;
