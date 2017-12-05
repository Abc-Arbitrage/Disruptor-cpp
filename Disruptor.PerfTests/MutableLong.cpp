#include "stdafx.h"
#include "MutableLong.h"


namespace Disruptor
{
namespace PerfTests
{

    MutableLong::MutableLong(std::int64_t value)
        : value(value)
    {
    }

} // namespace PerfTests
} // namespace Disruptor
