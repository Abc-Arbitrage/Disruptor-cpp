#pragma once

#include <cstdint>

#include "Disruptor.TestTools/Stopwatch.h"


namespace Disruptor
{
namespace PerfTests
{

    class IThroughputTest
    {
    public:
        virtual ~IThroughputTest() = default;

        virtual std::int64_t run(Stopwatch& stopwatch) = 0;

        virtual std::int32_t requiredProcessorCount() const = 0;
    };

} // namespace PerfTests
} // namespace Disruptor
