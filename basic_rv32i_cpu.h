#pragma once

#include "basic_mem.h"
#include "rv32i_cpu.h"

// TYPES: totally concrete. Tied to a specific memory implementation.

// A basic CPU is an Rv32iCpu with an MIntegerCore with memory implemented as MBasicMem.
using BasicRv32iCpu = Rv32iCpuMIntegerCore<MBasicMem>;

// An Rv32imCpu with an MIntegerCore with memory implemented as MBasicMem.
using BasicRv32imCpu = Rv32imCpuMIntegerCore<MBasicMem>;

// An Rv32icCpu with an MIntegerCore with memory implemented as MBasicMem.
using BasicRv32icCpu = Rv32icCpuMIntegerCore<MBasicMem>;
