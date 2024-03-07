#pragma once

#include "arviss/platforms/basic/mem.h"
#include "arviss/rv32/rv32.h"

namespace arviss::basic
{
    // A basic CPU is an Rv32iCpu with an IntegerCore with memory implemented as MemoryWithIO.
    using BasicRv32iCpu = Rv32iCpu<MemoryWithIO>;
    static_assert(IsRv32iCpu<BasicRv32iCpu>);

    // An Rv32imCpu with an IntegerCore with memory implemented as MemoryWithIO.
    using BasicRv32imCpu = Rv32imCpu<MemoryWithIO>;
    static_assert(IsRv32imCpu<BasicRv32imCpu>);

    // An Rv32icCpu with an IntegerCore with memory implemented as MemoryWithIO.
    using BasicRv32icCpu = Rv32icCpu<MemoryWithIO>;
    static_assert(IsRv32icCpu<BasicRv32icCpu>);

    // An Rv32imfCpu with a FloatCore with memory implemented as MemoryWithIO.
    using BasicRv32imfCpu = Rv32imfCpu<MemoryWithIO>;
    static_assert(IsRv32imfCpu<BasicRv32imfCpu>);

} // namespace arviss::basic
