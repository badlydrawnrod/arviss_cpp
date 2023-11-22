#pragma once

#include "basic_mem.h"
#include "rv32i_cpu.h"

// TYPES: totally concrete. Tied to a specific memory implementation. 

// A basic CPU is an Rv32iCpu with a specific memory implementation.
using BasicRv32iCpu = AbstractRV32iCpu<MBasicUncheckedMem>;
