#pragma once

#include "basic_mem.h"
#include "rv32i_cpu.h"

namespace arviss
{
    // A basic CPU is an Rv32iCpu with an IntegerCore with memory implemented as BasicMem.
    using BasicRv32iCpu = Rv32iCpuMIntegerCore<BasicMem>;

    // An Rv32imCpu with an IntegerCore with memory implemented as MBasicMem.
    using BasicRv32imCpu = Rv32imCpuMIntegerCore<BasicMem>;

    // An Rv32icCpu with an IntegerCore with memory implemented as BasicMem.
    using BasicRv32icCpu = Rv32icCpuMIntegerCore<BasicMem>;

} // namespace arviss
