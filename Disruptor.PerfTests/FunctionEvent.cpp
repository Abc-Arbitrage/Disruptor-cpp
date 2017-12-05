#include "stdafx.h"
#include "FunctionEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    const std::function< FunctionEvent() >& FunctionEvent::eventFactory()
    {
        static std::function< FunctionEvent() > result([] { return FunctionEvent(); });
        return result;
    }

} // namespace PerfTests
} // namespace Disruptor
