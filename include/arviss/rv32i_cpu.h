#pragma once

#include "arviss/core/concepts.h"
#include "arviss/core/mixins.h"
#include "arviss/rv32_dispatchers.h"
#include "arviss/rv32_executors.h"

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

    // An RV32imf CPU implementation for a floating point core. BYO floating point core. BYO memory.
    template<IsFloatCore T>
    using Rv32imfCpu = Rv32imfDispatcher<Rv32imfFloatCoreExecutor<T>>;

    // An RV32i CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32iCpuIntegerCore = Rv32iCpu<IntegerCore<Mem>>;

    // An RV32im CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imCpuIntegerCore = Rv32imCpu<IntegerCore<Mem>>;

    // An RV32ic CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32icCpuIntegerCore = Rv32icCpu<IntegerCore<Mem>>;

    // An RV32imf CPU implementation for a FloatCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imfCpuFloatCore = Rv32imfCpu<FloatCore<Mem>>;

} // namespace arviss
