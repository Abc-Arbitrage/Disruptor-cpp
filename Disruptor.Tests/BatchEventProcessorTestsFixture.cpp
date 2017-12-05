#include "stdafx.h"
#include "BatchEventProcessorTestsFixture.h"


namespace Disruptor
{
namespace Tests
{

    BatchEventProcessorTestsFixture::BatchEventProcessorTestsFixture()
        : m_countDownEvent(1)
    {
        auto eventFactory = []() { return StubEvent(-1); };

        m_ringBuffer = std::make_shared< RingBuffer< StubEvent > >(eventFactory, 16);
        m_sequenceBarrier = m_ringBuffer->newBarrier();

        m_batchHandlerMock = std::make_shared< testing::NiceMock< BatchHandlerMock< StubEvent > > >();
        m_excpetionHandlerMock = std::make_shared< testing::NiceMock< ExceptionHandlerMock< StubEvent > > >();
        m_batchEventProcessor = std::make_shared< BatchEventProcessor< StubEvent > >(m_ringBuffer, m_sequenceBarrier, m_batchHandlerMock);
    }

} // namespace Tests
} // namespace Disruptor
