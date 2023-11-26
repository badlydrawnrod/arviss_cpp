#pragma once

#include "core_concepts.h"
#include "core_mixins.h"
#include "rv32i_dispatcher.h"
#include "rv32i_executors.h"
#include "rv32ic_dispatcher.h"
#include "rv32im_dispatcher.h"

// An RV32i CPU implementation for an integer core. BYO integer core. BYO memory.
template<IsIntegerCore T>
using Rv32iCpu = MRv32iDispatcher<MRv32iIntegerCoreExecutor<T>>;

// An RV32im CPU implementation for an integer core. BYO integer core. BYO memory.
template<IsIntegerCore T>
using Rv32imCpu = MRv32imDispatcher<MRv32imIntegerCoreExecutor<T>>;

// An RV32ic CPU implementation for an integer core. BYO integer core. BYO memory.
template<IsIntegerCore T>
using Rv32icCpu = MRv32icDispatcher<MRv32icIntegerCoreExecutor<T>>;

// An RV32i CPU implementation for an MIntegerCore. BYO memory.
template<HasMemory Mem>
using Rv32iCpuMIntegerCore = Rv32iCpu<MIntegerCore<Mem>>;

// An RV32im CPU implementation for an MIntegerCore. BYO memory.
template<HasMemory Mem>
using Rv32imCpuMIntegerCore = Rv32imCpu<MIntegerCore<Mem>>;

// An RV32ic CPU implementation for an MIntegerCore. BYO memory.
template<HasMemory Mem>
using Rv32icCpuMIntegerCore = Rv32icCpu<MIntegerCore<Mem>>;
