#pragma once

#include <string>

#include "Disruptor.PerfTests/ILatencyTest.h"
#include "Disruptor.PerfTests/IThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    template <class TTest>
    struct TestInfo
    {
        std::string name;
        std::shared_ptr< TTest > instance;
    };


    using ThroughputTestInfo = TestInfo< IThroughputTest >;
    using LatencyTestInfo = TestInfo< ILatencyTest >;

} // namespace PerfTests
} // namespace Disruptor
