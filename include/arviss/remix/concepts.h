#pragma once

#include "arviss/common/types.h"
#include "arviss/core/concepts.h"
#include "arviss/remix/encoder.h"

namespace arviss::remix
{
    template<typename T>
    concept IsRemixDispatchable = IsRv32iInstructionHandler<T> && HasFetch<T> && HasUncheckedMemory<T>;

} // namespace arviss::remix
