#pragma once

#include <cstdint>
#include <string>


namespace Disruptor
{

    class IEventProcessor;

namespace PerfTests
{
namespace PerfTestUtil
{

    std::int64_t accumulatedAddition(std::int64_t iterations);

    void failIf(std::int64_t a, std::int64_t b, const std::string& message = std::string());

    void failIfNot(std::int64_t a, std::int64_t b, const std::string& message = std::string());

    void waitForEventProcessorSequence(std::int64_t expectedCount, const std::shared_ptr< IEventProcessor >& batchEventProcessor);

    std::string utcDateToString();
    
} // namespace PerfTestUtil
} // namespace PerfTests
} // namespace Disruptor
