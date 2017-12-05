#include "stdafx.h"

#include "Disruptor/ArgumentOutOfRangeException.h"
#include "Disruptor/BusySpinWaitStrategy.h"
#include "Disruptor/EventPoller.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/SleepingWaitStrategy.h"
#include "Disruptor/RingBuffer.h"

#include "DataProviderMock.h"
#include "SequencerMock.h"


using namespace Disruptor;
using namespace Disruptor::Tests;

BOOST_AUTO_TEST_SUITE(EventPollerTests)

BOOST_AUTO_TEST_CASE(ShouldPollForEvents)
{
    typedef std::int32_t TestDataType;

    auto pollSequence = std::make_shared< Sequence >();
    auto bufferSequence = std::make_shared< Sequence >();
    auto gatingSequence = std::make_shared<  Sequence >();

    auto sequencerMock = std::make_shared< testing::NiceMock< SequencerMock< TestDataType > > >();

    auto handled = false;
    auto handler = [&](TestDataType&, std::int64_t, bool) -> bool
    {
        handled = true;
        return false;
    };

    auto providerMock = std::make_shared< testing::NiceMock< DataProviderMock< TestDataType > > >();
    auto poller = EventPoller< TestDataType >::newInstance(providerMock, sequencerMock, pollSequence, bufferSequence, { gatingSequence });
    auto event = std::int32_t(42);
   
    auto states = PollState::Idle;

    EXPECT_CALL(*sequencerMock, cursor()).WillRepeatedly(testing::Invoke([&]()
    {
        switch (states)
        {
        case PollState::Processing:
            return 0L;
        case PollState::Gating:
            return 0L;
        case PollState::Idle:
            return -1L;
        default:
            DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION(states);
        }
    }));

    EXPECT_CALL(*sequencerMock, getHighestPublishedSequence(0L, -1L)).WillRepeatedly(testing::Return(-1L));
    EXPECT_CALL(*sequencerMock, getHighestPublishedSequence(0L, 0L)).WillRepeatedly(testing::Return(0L));

    EXPECT_CALL(*providerMock, indexer(0)).WillRepeatedly(testing::ReturnRef(event));

    // Initial State - nothing published.
    states = PollState::Idle;
    BOOST_CHECK_EQUAL(poller->poll(handler), PollState::Idle);

    // Publish Event.
    states = PollState::Gating;
    bufferSequence->incrementAndGet();
    BOOST_CHECK_EQUAL(poller->poll(handler), PollState::Gating);

    states = PollState::Processing;
    gatingSequence->incrementAndGet();
    BOOST_CHECK_EQUAL(poller->poll(handler), PollState::Processing);

    BOOST_CHECK_EQUAL(handled, true);
}


BOOST_AUTO_TEST_CASE(ShouldSuccessfullyPollWhenBufferIsFull)
{
    typedef std::int32_t DataType;

    auto handled = 0;
    auto handler = [&](std::vector< DataType >&, std::int64_t, bool) -> bool
    {
        handled++;
        return true;
    };

    auto factory = [] { return std::vector< DataType >(1); };

    auto ringBuffer = RingBuffer< std::vector< DataType > >::createMultiProducer(factory, 0x4, std::make_shared< SleepingWaitStrategy >());

    auto poller = ringBuffer->newPoller({});
    ringBuffer->addGatingSequences({ poller->sequence() });

    const auto count = 4;

    for (auto i = 1; i <= count; ++i)
    {
        auto next = ringBuffer->next();
        (*ringBuffer)[next][0] = i; //sucks
        ringBuffer->publish(next);
    }

    // think of another thread
    poller->poll(handler);

    BOOST_CHECK_EQUAL(handled, 4);
}

BOOST_AUTO_TEST_SUITE_END()
