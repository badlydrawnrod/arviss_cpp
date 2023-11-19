#pragma once

#include "concepts.h"
#include "dispatcher.h"

// An RV32i CPU with registers, basic memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher.
template<IsIntCpu T>
using RV32iCpu = MRv32iDispatcher<MRv32iHandler<T>>;
