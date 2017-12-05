#pragma once

#include <gmock/gmock.h>

#include "Disruptor/ConsumerRepository.h"
#include "Disruptor/Sequence.h"

#include "Disruptor.Tests/EventProcessorMock.h"
#include "Disruptor.Tests/SequenceBarrierMock.h"
#include "Disruptor.Tests/SleepingEventHandler.h"


namespace Disruptor
{
namespace Tests
{

    struct ConsumerRepositoryTestsFixture
    {
        ConsumerRepositoryTestsFixture();

        std::shared_ptr< EventProcessorMock > m_eventProcessor1;
        std::shared_ptr< EventProcessorMock > m_eventProcessor2;
        std::shared_ptr< SleepingEventHandler > m_handler1;
        std::shared_ptr< SleepingEventHandler > m_handler2;
        std::shared_ptr< SequenceBarrierMock > m_barrier1;
        std::shared_ptr< SequenceBarrierMock > m_barrier2;
        ConsumerRepository< TestEvent > m_consumerRepository;
    };

} // namespace Tests
} // namespace Disruptor
