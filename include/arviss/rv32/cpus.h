#pragma once

#include "arviss/core/concepts.h"
#include "arviss/core/mixins.h"
#include "arviss/rv32/dispatchers.h"
#include "arviss/rv32/executors.h"

namespace arviss
{
    // An RV32i CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32iCpuIntegerCore = Rv32iDispatcher<Rv32iIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32im CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imCpuIntegerCore = Rv32imDispatcher<Rv32imIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32ic CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32icCpuIntegerCore = Rv32icDispatcher<Rv32icIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32imf CPU implementation for a FloatCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imfCpuFloatCore = Rv32imfDispatcher<Rv32imfFloatCoreExecutor<FloatCore<Mem>>>;

} // namespace arviss
