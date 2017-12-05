#include "stdafx.h"

#include "BatchEventProcessorTestsFixture.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(BatchEventProcessorTests);

BOOST_FIXTURE_TEST_CASE(ShouldThrowExceptionOnSettingNullExceptionHandler, BatchEventProcessorTestsFixture)
{
    BOOST_CHECK_THROW(m_batchEventProcessor->setExceptionHandler(nullptr), ArgumentNullException);
}

BOOST_FIXTURE_TEST_CASE(ShouldCallMethodsInLifecycleOrder, BatchEventProcessorTestsFixture)
{
    EXPECT_CALL(*m_batchHandlerMock, onEvent((*m_ringBuffer)[0], 0, true)).WillOnce(testing::Invoke([this](StubEvent&, std::int64_t, bool)
    {
        m_countDownEvent.signal();
    }));

    std::thread thread([this] { m_batchEventProcessor->run(); });

    BOOST_CHECK_EQUAL(-1, m_batchEventProcessor->sequence()->value());

    m_ringBuffer->publish(m_ringBuffer->next());

    BOOST_CHECK_EQUAL(m_countDownEvent.wait(std::chrono::milliseconds(50)), true);
    m_batchEventProcessor->halt();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(ShouldCallMethodsInLifecycleOrderForBatch, BatchEventProcessorTestsFixture)
{
    EXPECT_CALL(*m_batchHandlerMock, onEvent((*m_ringBuffer)[0], 0, false)).Times(1);
    EXPECT_CALL(*m_batchHandlerMock, onEvent((*m_ringBuffer)[1], 1, false)).Times(1);
    EXPECT_CALL(*m_batchHandlerMock, onEvent((*m_ringBuffer)[2], 2, true)).WillOnce(testing::Invoke([this](StubEvent&, std::int64_t, bool)
    {
        m_countDownEvent.signal();
    }));

    m_ringBuffer->publish(m_ringBuffer->next());
    m_ringBuffer->publish(m_ringBuffer->next());
    m_ringBuffer->publish(m_ringBuffer->next());

    std::thread thread([this] { m_batchEventProcessor->run(); });

    BOOST_CHECK_EQUAL(m_countDownEvent.wait(std::chrono::milliseconds(50)), true);
    m_batchEventProcessor->halt();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(ShouldCallExceptionHandlerOnUncaughtException, BatchEventProcessorTestsFixture)
{
    InvalidOperationException ex("BatchEventProcessorTests.ShouldCallExceptionHandlerOnUncaughtException");
    m_batchEventProcessor->setExceptionHandler(m_excpetionHandlerMock);
    
    EXPECT_CALL(*m_batchHandlerMock, onEvent((*m_ringBuffer)[0], 0, true)).WillRepeatedly(testing::Throw(ex));

    EXPECT_CALL(*m_excpetionHandlerMock, handleEventException(testing::_, 0, (*m_ringBuffer)[0]))
        .WillRepeatedly(testing::Invoke([&](const std::exception& lException, std::int64_t, StubEvent&)
        {
            BOOST_CHECK_EQUAL(lException.what(), ex.what());
            m_countDownEvent.signal();
        }));

    std::thread thread([this] { m_batchEventProcessor->run(); });

    m_ringBuffer->publish(m_ringBuffer->next());

    BOOST_CHECK_EQUAL(m_countDownEvent.wait(std::chrono::milliseconds(50)), true);
    m_batchEventProcessor->halt();
    thread.join();
}

BOOST_AUTO_TEST_SUITE_END()
