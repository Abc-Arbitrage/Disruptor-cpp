#pragma once

#include <cstdint>


namespace Disruptor
{
namespace PerfTests
{

    struct PaddedLong
    {
        std::int64_t value = 0;
        char _padding[54];
    };

} // namespace PerfTests
} // namespace Disruptor
