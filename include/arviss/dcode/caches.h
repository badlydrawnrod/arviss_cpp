#pragma once

#include "arviss/common/types.h"
#include "arviss/dcode/encoder.h"

#include <vector>

namespace arviss
{
    class SimpleCache
    {
        static const size_t defaultCacheSize = 8192;

        std::vector<DCode> cache_ = std::vector<DCode>(defaultCacheSize);

    public:
        SimpleCache() = default;
        SimpleCache(size_t size) : cache_(size) {}

        // DCode Get(Address addr) { return cache_[addr]; }
        const DCode& Get(Address addr) const { return cache_[addr]; }
        void Put(Address addr, DCode e) { cache_[addr] = e; }
    };

} // namespace arviss