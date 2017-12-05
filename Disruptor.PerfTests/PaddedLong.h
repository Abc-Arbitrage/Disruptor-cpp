#pragma once

#include <cstdint>


namespace Disruptor
{
namespace PerfTests
{

    //[StructLayout(LayoutKind.Explicit, Size = 64)]
    struct PaddedLong
    {
        //[FieldOffset(0)]
        std::int64_t value = 0;
    };

} // namespace PerfTests
} // namespace Disruptor
