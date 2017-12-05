#pragma once

#include <gmock/gmock.h>

#include "Disruptor/IEventProcessor.h"


namespace Disruptor
{
namespace Tests
{

    class EventProcessorMock : public IEventProcessor
    {
    public:
        MOCK_CONST_METHOD0(sequence, std::shared_ptr< ISequence >());

        MOCK_METHOD0(halt, void());
        MOCK_METHOD0(run, void());

        MOCK_CONST_METHOD0(isRunning, bool());
    };

} // namespace Tests
} // namespace Disruptor
