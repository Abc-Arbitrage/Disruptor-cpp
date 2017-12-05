#pragma once

#include <cstdint>

#include "Disruptor.TestTools/LatencyRecorder.h"
#include "Disruptor.TestTools/Stopwatch.h"


namespace Disruptor
{
namespace PerfTests
{

    class ILatencyTest
    {
    public:
        virtual ~ILatencyTest() = default;

        virtual void run(Stopwatch& stopwatch, const std::shared_ptr< Tests::LatencyRecorder >& latencyRecorder) = 0;

        virtual std::int32_t requiredProcessorCount() const = 0;
    };

} // namespace PerfTests
} // namespace Disruptor
