#pragma once

#include "core_concepts.h"
#include "core_mixins.h"
#include "rv32i_dispatcher.h"
#include "rv32i_executors.h"
#include "rv32ic_dispatcher.h"
#include "rv32im_dispatcher.h"

namespace arviss
{
    // An RV32i CPU implementation for an integer core. BYO integer core. BYO memory.
    template<IsIntegerCore T>
    using Rv32iCpu = Rv32iDispatcher<Rv32iIntegerCoreExecutor<T>>;

    // An RV32im CPU implementation for an integer core. BYO integer core. BYO memory.
    template<IsIntegerCore T>
    using Rv32imCpu = Rv32imDispatcher<Rv32imIntegerCoreExecutor<T>>;

    // An RV32ic CPU implementation for an integer core. BYO integer core. BYO memory.
    template<IsIntegerCore T>
    using Rv32icCpu = Rv32icDispatcher<Rv32icIntegerCoreExecutor<T>>;

    // An RV32i CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32iCpuMIntegerCore = Rv32iCpu<IntegerCore<Mem>>;

    // An RV32im CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imCpuMIntegerCore = Rv32imCpu<IntegerCore<Mem>>;

    // An RV32ic CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32icCpuMIntegerCore = Rv32icCpu<IntegerCore<Mem>>;

} // namespace arviss
