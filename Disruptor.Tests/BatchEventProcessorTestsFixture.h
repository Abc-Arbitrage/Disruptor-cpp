#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.TestTools/CountdownEvent.h"

#include "Disruptor.Tests/BatchHandlerMock.h"
#include "Disruptor.Tests/ExceptionHandlerMock.h"
#include "Disruptor.Tests/StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    struct BatchEventProcessorTestsFixture
    {
        BatchEventProcessorTestsFixture();

        std::shared_ptr< RingBuffer< StubEvent > > m_ringBuffer;
        std::shared_ptr< ISequenceBarrier > m_sequenceBarrier;
        std::shared_ptr< BatchHandlerMock< StubEvent > > m_batchHandlerMock;
        std::shared_ptr< ExceptionHandlerMock< StubEvent > > m_excpetionHandlerMock;
        std::shared_ptr< BatchEventProcessor< StubEvent > > m_batchEventProcessor;
        CountdownEvent m_countDownEvent;
    };

} // namespace Tests
} // namespace Disruptor
