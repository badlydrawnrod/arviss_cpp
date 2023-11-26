#pragma once

#include "core_concepts.h"
#include "core_mixins.h"
#include "rv32i_dispatcher.h"
#include "rv32i_instruction_handler.h"
#include "rv32ic_dispatcher.h"
#include "rv32im_dispatcher.h"

// An RV32i CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher. RV32i specific.
template<IsIntegerCore T>
using RV32iCpu = MRv32iDispatcher<MRv32iHandler<T>>;

// A mixin RV32i CPU. BYO memory. RV32i specific.
// A specialization of RV32iCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using AbstractRV32iCpu = RV32iCpu<MIntegerCore<Mem>>;

// An RV32im CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher. RV32i specific.
template<IsIntegerCore T>
using RV32imCpu = MRv32imDispatcher<MRv32imHandler<T>>;

// A mixin RV32i CPU. BYO memory. RV32im specific.
// A specialization of RV32imCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using AbstractRV32imCpu = RV32imCpu<MIntegerCore<Mem>>;

// An RV32ic CPU with registers, memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher. RV32i specific.
template<IsIntegerCore T>
using RV32icCpu = MRv32icDispatcher<MRv32icHandler<T>>;

// A mixin RV32ic CPU. BYO memory. RV32ic specific.
// A specialization of RV32icCpu for a MIntegerCore with an unspecified memory implementation.
template<HasMemory Mem>
using AbstractRV32icCpu = RV32icCpu<MIntegerCore<Mem>>;
