#pragma once

#include "basic_mem.h"
#include "rv32i_cpu.h"

// TYPES: totally concrete. Tied to a specific memory implementation.

// A basic CPU is an Rv32iCpu with a specific memory implementation.
using BasicRv32iCpu = AbstractRV32iCpu<MBasicMem>;

// An Rv32imCpu with a specific memory implementation.
using BasicRv32imCpu = AbstractRV32imCpu<MBasicMem>;

// An Rv32icCpu with a specific memory implementation.
using BasicRv32icCpu = AbstractRV32icCpu<MBasicMem>;
