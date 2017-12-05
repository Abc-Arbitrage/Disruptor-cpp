#include "stdafx.h"
#include "ConsumerRepositoryTestsFixture.h"


namespace Disruptor
{
namespace Tests
{

    ConsumerRepositoryTestsFixture::ConsumerRepositoryTestsFixture()
        : m_eventProcessor1(std::make_shared< testing::NiceMock< EventProcessorMock > >())
        , m_eventProcessor2(std::make_shared< testing::NiceMock< EventProcessorMock > >())
        , m_handler1(std::make_shared< SleepingEventHandler >())
        , m_handler2(std::make_shared< SleepingEventHandler >())
        , m_barrier1(std::make_shared< testing::NiceMock< SequenceBarrierMock > >())
        , m_barrier2(std::make_shared< testing::NiceMock< SequenceBarrierMock > >())
    {
        auto sequence1 = std::make_shared< Sequence >();
        auto sequence2 = std::make_shared< Sequence >();

        ON_CALL(*m_eventProcessor1, sequence()).WillByDefault(testing::Return(sequence1));
        ON_CALL(*m_eventProcessor1, isRunning()).WillByDefault(testing::Return(true));
        ON_CALL(*m_eventProcessor2, sequence()).WillByDefault(testing::Return(sequence1));
        ON_CALL(*m_eventProcessor2, isRunning()).WillByDefault(testing::Return(true));
    }

} // namespace Tests
} // namespace Disruptor
