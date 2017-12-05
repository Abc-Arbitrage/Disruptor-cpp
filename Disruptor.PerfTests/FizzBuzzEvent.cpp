#include "stdafx.h"
#include "FizzBuzzEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    void FizzBuzzEvent::reset()
    {
        value = 0;
        fizz = false;
        buzz = false;
    }

    const std::function< FizzBuzzEvent() >& FizzBuzzEvent::eventFactory()
    {
        static std::function< FizzBuzzEvent() > result([] { return FizzBuzzEvent(); });
        return result;
    }

} // namespace PerfTests
} // namespace Disruptor
