#include "stdafx.h"
#include "ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    const std::function< ValueEvent() >& ValueEvent::eventFactory()
    {
        static std::function< ValueEvent() > result([] { return ValueEvent(); });
        return result;
    }

} // namespace PerfTests
} // namespace Disruptor
