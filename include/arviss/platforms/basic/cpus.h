#pragma once

#include "arviss/platforms/basic/mem.h"
#include "arviss/rv32/cpus.h"

namespace arviss
{
    // A basic CPU is an Rv32iCpu with an IntegerCore with memory implemented as BasicMem.
    using BasicRv32iCpu = Rv32iCpuIntegerCore<BasicMem>;

    static_assert(IsRv32iCpu<BasicRv32iCpu>);

    // An Rv32imCpu with an IntegerCore with memory implemented as BasicMem.
    using BasicRv32imCpu = Rv32imCpuIntegerCore<BasicMem>;

    static_assert(IsRv32imCpu<BasicRv32imCpu>);

    // An Rv32icCpu with an IntegerCore with memory implemented as BasicMem.
    using BasicRv32icCpu = Rv32icCpuIntegerCore<BasicMem>;

    static_assert(IsRv32icCpu<BasicRv32icCpu>);

    // An Rv32imfCpu with a FloatCore with memory implemented as BasicMem.
    using BasicRv32imfCpu = Rv32imfCpuFloatCore<BasicMem>;

    static_assert(IsRv32imfCpu<BasicRv32imfCpu>);

} // namespace arviss
