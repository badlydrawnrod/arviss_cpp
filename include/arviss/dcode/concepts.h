#pragma once

#include "arviss/common/types.h"
#include "arviss/dcode/arviss_encoder.h"

namespace arviss
{
    template<typename T>
    concept IsCache = requires(T t, Encoding e) {
        e = t.Get(Address{}); // Read from the cache at a given address.
        t.Put(Address{}, e);  // Write to the cache at a given address.
    };
} // namespace arviss
