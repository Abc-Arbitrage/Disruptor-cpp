#pragma once

#include <cstdint>


namespace Disruptor
{
namespace PerfTests
{

    class MutableLong
    {
    public:
        explicit MutableLong(std::int64_t value);

        std::int64_t value;
    };

} // namespace PerfTests
} // namespace Disruptor
