#pragma once

#include <cstdint>
#include <functional>

namespace Disruptor
{
namespace PerfTests
{

    class ValueEvent
    {
    public:
        static const std::function< ValueEvent() >& eventFactory();

        std::int64_t value;
    };

} // namespace PerfTests
} // namespace Disruptor
