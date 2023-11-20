#pragma once

#include "concepts.h"

// TYPES: generic integer CPU.

// A mixin CPU with integer registers, memory, and a fetch cycle implementation on that memory. Not RV32i specific.
// Satisfies: IsIntCpu
template<HasMemory Mem>
struct MIntCpu : public MXRegisters, public MFetch<Mem>
{
};
