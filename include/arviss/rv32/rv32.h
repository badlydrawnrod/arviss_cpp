#pragma once

#include "arviss/arviss.h"
#include "arviss/rv32/dispatchers.h"
#include "arviss/rv32/executors.h"

namespace arviss
{
    // An RV32i CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32iCpu = Rv32iDispatcher<Rv32iIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32im CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imCpu = Rv32imDispatcher<Rv32imIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32ic CPU implementation for an IntegerCore. BYO memory.
    template<HasMemory Mem>
    using Rv32icCpu = Rv32icDispatcher<Rv32icIntegerCoreExecutor<IntegerCore<Mem>>>;

    // An RV32imf CPU implementation for a FloatCore. BYO memory.
    template<HasMemory Mem>
    using Rv32imfCpu = Rv32imfDispatcher<Rv32imfFloatCoreExecutor<FloatCore<Mem>>>;

} // namespace arviss
