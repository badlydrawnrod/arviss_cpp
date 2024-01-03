#pragma once

#include "arviss/common/types.h"
#include "arviss/dcode/encoder.h"

namespace arviss
{
    template<typename T>
    concept IsCache = requires(T t, DCode e) {
        e = t.Get(Address{}); // Read from the cache at a given address.
        t.Put(Address{}, e);  // Write to the cache at a given address.
    };

    // For a DCodeDispatcher to work, T must satisfy the following requirements, encoded in IsDCodeDispatchable.
    // - IsRv32iInstructionHandler<T> so that it can call Add(), Beq(), etc.
    // - HasFetch<T> so that it can call Transfer() and SetNextPc()
    //
    // Further requirements for RV32 extensions are guarded by `if constexpr` on instruction handlers for the relevant
    // concept.
    template<typename T>
    concept IsDCodeDispatchable = IsRv32iInstructionHandler<T> && HasFetch<T>;

} // namespace arviss
