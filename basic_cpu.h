#pragma once

#include "arviss.h"
#include "basic_mem.h"
#include "rv32i_cpu.h"

// A mixin CPU with integer registers and basic memory with a fetch cycle implementation.
struct MIntCpu : public MXRegisters,      // it has registers
                 public MFetch<MBasicMem> // it has memory with a fetch cycle implemented on it
{
};

// A basic CPU applies the Rv32iCpu behaviours to an MIntCpu.
using BasicCpu = RV32iCpu<MIntCpu>;
