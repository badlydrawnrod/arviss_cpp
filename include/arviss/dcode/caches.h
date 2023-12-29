#pragma once

#include "arviss/common/types.h"
#include "arviss/dcode/arviss_encoder.h"

#include <vector>

namespace arviss
{
    class SimpleCache
    {
        static const size_t defaultCacheSize = 8192;

        std::vector<Encoding> cache_ = std::vector<Encoding>(defaultCacheSize);

    public:
        SimpleCache() = default;
        SimpleCache(size_t size) : cache_(size) {}

        Encoding Get(Address addr) { return cache_[addr]; }
        void Put(Address addr, Encoding e) { cache_[addr] = e; }
    };

} // namespace arviss