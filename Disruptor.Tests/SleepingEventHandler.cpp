#include "stdafx.h"
#include "SleepingEventHandler.h"


namespace Disruptor
{
namespace Tests
{

    void SleepingEventHandler::onEvent(TestEvent& /*data*/, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

} // namespace Tests
} // namespace Disruptor
