#pragma once

#include <string>

#include "Disruptor.PerfTests/ILatencyTest.h"
#include "Disruptor.PerfTests/IThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    template <class TTest>
    struct TestFactory
    {
        std::string name;
        std::function<std::shared_ptr< TTest >()> factory;
    };


    using ThroughputTestInfo = TestFactory< IThroughputTest >;
    using LatencyTestInfo = TestFactory< ILatencyTest >;

} // namespace PerfTests
} // namespace Disruptor
