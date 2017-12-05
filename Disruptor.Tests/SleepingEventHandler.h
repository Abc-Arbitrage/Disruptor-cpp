#pragma once

#include "Disruptor/IEventHandler.h"

#include "Disruptor.Tests/TestEvent.h"


namespace Disruptor
{
namespace Tests
{
    
    class SleepingEventHandler : public IEventHandler< TestEvent >
    {
    public:
        void onEvent(TestEvent& data, std::int64_t sequence, bool endOfBatch) override;
    };

} // namespace Tests
} // namespace Disruptor
