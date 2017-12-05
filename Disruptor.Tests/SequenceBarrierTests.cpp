#include "stdafx.h"

#include "Disruptor/AlertException.h"
#include "Disruptor/IgnoreExceptionHandler.h"
#include "Disruptor/Sequence.h"

#include "SequenceBarrierTestsFixture.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_FIXTURE_TEST_SUITE(SequenceBarrierTests, SequenceBarrierTestsFixture)

BOOST_AUTO_TEST_CASE(ShouldWaitForWorkCompleteWhereCompleteWorkThresholdIsAhead)
{
    const std::int32_t expectedNumberMessages = 10;
    const std::int32_t expectedWorkSequence = 9;
    fillRingBuffer(expectedNumberMessages);

    auto sequence1 = std::make_shared< Sequence >(expectedNumberMessages);
    auto sequence2 = std::make_shared< Sequence >(expectedNumberMessages);
    auto sequence3 = std::make_shared< Sequence >(expectedNumberMessages);

    EXPECT_CALL(*m_eventProcessorMock1, sequence()).WillOnce(testing::Return(sequence1));
    EXPECT_CALL(*m_eventProcessorMock2, sequence()).WillOnce(testing::Return(sequence2));
    EXPECT_CALL(*m_eventProcessorMock3, sequence()).WillOnce(testing::Return(sequence3));

    auto dependencyBarrier = m_ringBuffer->newBarrier({ m_eventProcessorMock1->sequence(),
                                                        m_eventProcessorMock2->sequence(),
                                                        m_eventProcessorMock3->sequence() });

    auto completedWorkSequence = dependencyBarrier->waitFor(expectedWorkSequence);
    BOOST_CHECK_GE(completedWorkSequence, expectedWorkSequence);
}

BOOST_AUTO_TEST_CASE(ShouldWaitForWorkCompleteWhereAllWorkersAreBlockedOnRingBuffer)
{
    const std::int32_t expectedNumberMessages = 10;
    fillRingBuffer(expectedNumberMessages);

    std::vector< std::shared_ptr< IEventProcessor > > workers(3);
    for (auto i = 0u; i < workers.size(); ++i)
    {
        workers[i] = std::make_shared< StubEventProcessor >(expectedNumberMessages - 1);
    }

    auto dependencyBarrier = m_ringBuffer->newBarrier(Util::getSequencesFor(workers));
    std::thread([=]
    {
        auto sequence = m_ringBuffer->next();
        (*m_ringBuffer)[sequence].value(static_cast< int >(sequence));
        m_ringBuffer->publish(sequence);

        for (auto&& stubWorker : workers)
        {
            stubWorker->sequence()->setValue(sequence);
        }
    }).detach();

    std::int64_t expectedWorkSequence = expectedNumberMessages;
    auto completedWorkSequence = dependencyBarrier->waitFor(expectedWorkSequence);
    BOOST_CHECK_GE(completedWorkSequence, expectedWorkSequence);
}

BOOST_AUTO_TEST_CASE(ShouldInterruptDuringBusySpin)
{
    const std::int32_t expectedNumberMessages = 10;
    fillRingBuffer(expectedNumberMessages);

    auto signal = std::make_shared< CountdownEvent >(3);
    auto sequence1 = std::make_shared< CountDownEventSequence >(8L, signal);
    auto sequence2 = std::make_shared< CountDownEventSequence >(8L, signal);
    auto sequence3 = std::make_shared< CountDownEventSequence >(8L, signal);

    EXPECT_CALL(*m_eventProcessorMock1, sequence()).WillOnce(testing::Return(sequence1));
    EXPECT_CALL(*m_eventProcessorMock2, sequence()).WillOnce(testing::Return(sequence2));
    EXPECT_CALL(*m_eventProcessorMock3, sequence()).WillOnce(testing::Return(sequence3));

    auto sequenceBarrier = m_ringBuffer->newBarrier(Util::getSequencesFor({ m_eventProcessorMock1,
                                                                            m_eventProcessorMock2,
                                                                            m_eventProcessorMock3 }));

    auto alerted = false;
    auto future = std::async(std::launch::async, [=, &alerted]()
    {
        try
        {
            sequenceBarrier->waitFor(expectedNumberMessages - 1);
        }
        catch (AlertException&)
        {
            alerted = true;
        }
    });

    signal->wait(std::chrono::seconds(3));
    sequenceBarrier->alert();
    future.wait();

    BOOST_CHECK_MESSAGE(alerted == true, "Thread was not interrupted");
}

BOOST_AUTO_TEST_CASE(ShouldWaitForWorkCompleteWhereCompleteWorkThresholdIsBehind)
{
    const std::int32_t expectedNumberMessages = 10;
    fillRingBuffer(expectedNumberMessages);

    std::vector< std::shared_ptr< IEventProcessor > > eventProcessors(3);
    for (auto i = 0u; i < eventProcessors.size(); ++i)
    {
        eventProcessors[i] = std::make_shared< StubEventProcessor >(expectedNumberMessages - 2);
    }

    auto eventProcessorBarrier = m_ringBuffer->newBarrier(Util::getSequencesFor(eventProcessors));

    std::async(std::launch::async, [=]
    {
        for (auto&& stubWorker : eventProcessors)
        {
            stubWorker->sequence()->setValue(stubWorker->sequence()->value() + 1);
        }
    }).wait();

    std::int64_t expectedWorkSequence = expectedNumberMessages - 1;
    auto completedWorkSequence = eventProcessorBarrier->waitFor(expectedWorkSequence);
    BOOST_CHECK_GE(completedWorkSequence, expectedWorkSequence);
}

BOOST_AUTO_TEST_CASE(ShouldSetAndClearAlertStatus)
{
    auto sequenceBarrier = m_ringBuffer->newBarrier();
    BOOST_CHECK_EQUAL(sequenceBarrier->isAlerted(), false);

    sequenceBarrier->alert();
    BOOST_CHECK_EQUAL(sequenceBarrier->isAlerted(), true);

    sequenceBarrier->clearAlert();
    BOOST_CHECK_EQUAL(sequenceBarrier->isAlerted(), false);
}

BOOST_AUTO_TEST_SUITE_END()
