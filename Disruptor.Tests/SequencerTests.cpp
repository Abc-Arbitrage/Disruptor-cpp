#include "stdafx.h"

#include <boost/mpl/vector.hpp>

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "SequencerFixture.h"
#include "WaitStrategyMock.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


typedef boost::mpl::vector< MultiProducerSequencer< int >, SingleProducerSequencer< int > > Sequencers;

BOOST_AUTO_TEST_SUITE(SequencerTests)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldStartWithInitialValue, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_EQUAL(this->m_sequencer->next(), 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldBatchClaim, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_EQUAL(this->m_sequencer->next(4), 3);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldIndicateHasAvailableCapacity, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(1), true);
    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(this->m_bufferSize), true);
    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(this->m_bufferSize + 1), false);

    this->m_sequencer->publish(this->m_sequencer->next());

    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(this->m_bufferSize - 1), true);
    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(this->m_bufferSize), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldIndicateNoAvailableCapacity, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    auto sequence = this->m_sequencer->next(this->m_bufferSize);
    this->m_sequencer->publish(sequence - (this->m_bufferSize - 1), sequence);

    BOOST_CHECK_EQUAL(this->m_sequencer->hasAvailableCapacity(1), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldHoldUpPublisherWhenBufferIsFull, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    auto sequence = this->m_sequencer->next(this->m_bufferSize);
    this->m_sequencer->publish(sequence - (this->m_bufferSize - 1), sequence);

    auto waitingSignal = std::make_shared< ManualResetEvent >(false);
    auto doneSignal = std::make_shared< ManualResetEvent >(false);

    auto expectedFullSequence = Sequence::InitialCursorValue + this->m_sequencer->bufferSize();
    BOOST_CHECK_EQUAL(this->m_sequencer->cursor(), expectedFullSequence);

    std::thread([=]
    {
        waitingSignal->set();

        auto next = this->m_sequencer->next();
        this->m_sequencer->publish(next);

        doneSignal->set();
    }).detach();

    waitingSignal->wait(std::chrono::milliseconds(500));
    BOOST_CHECK_EQUAL(this->m_sequencer->cursor(), expectedFullSequence);

    this->m_gatingSequence->setValue(Sequence::InitialCursorValue + 1L);

    doneSignal->wait(std::chrono::milliseconds(500));
    BOOST_CHECK_EQUAL(this->m_sequencer->cursor(), expectedFullSequence + 1L);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldThrowInsufficientCapacityExceptionWhenSequencerIsFull, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    for (auto i = 0; i < this->m_bufferSize; ++i)
    {
        this->m_sequencer->next();
    }

    BOOST_CHECK_THROW(this->m_sequencer->tryNext(), InsufficientCapacityException);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldCalculateRemainingCapacity, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    BOOST_CHECK_EQUAL(this->m_sequencer->getRemainingCapacity(), this->m_bufferSize);
    
    for (auto i = 1; i < this->m_bufferSize; ++i)
    {
        this->m_sequencer->next();
        BOOST_CHECK_EQUAL(this->m_sequencer->getRemainingCapacity(), this->m_bufferSize - i);
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShoundNotBeAvailableUntilPublished, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    auto next = this->m_sequencer->next(6);

    for (auto i = 0; i <= 5; i++)
    {
        BOOST_CHECK_EQUAL(this->m_sequencer->isAvailable(i), false);
    }

    this->m_sequencer->publish(next - (6 - 1), next);
    
    for (auto i = 0; i <= 5; i++)
    {
        BOOST_CHECK_EQUAL(this->m_sequencer->isAvailable(i), true);
    }

    BOOST_CHECK_EQUAL(this->m_sequencer->isAvailable(6), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotifyWaitStrategyOnPublish, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    auto waitStrategyMock = std::make_shared< testing::NiceMock< WaitStrategyMock > >();
    auto sequencer = std::make_shared< TSequencer >(this->m_bufferSize, waitStrategyMock);

    EXPECT_CALL(*waitStrategyMock, signalAllWhenBlocking()).Times(1);

    sequencer->publish(sequencer->next());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotifyWaitStrategyOnPublishBatch, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    auto waitStrategyMock = std::make_shared< testing::NiceMock< WaitStrategyMock > >();
    auto sequencer = std::make_shared< TSequencer >(this->m_bufferSize, waitStrategyMock);

    EXPECT_CALL(*waitStrategyMock, signalAllWhenBlocking()).Times(1);

    auto next = sequencer->next(4);
    sequencer->publish(next - (4 - 1), next);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldWaitOnPublication, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    auto barrier = this->m_sequencer->newBarrier({});

    auto next = this->m_sequencer->next(10);
    auto lo = next - (10 - 1);
    auto mid = next - 5;

    for (auto l = lo; l < mid; ++l)
    {
       this->m_sequencer->publish(l);
    }

    BOOST_CHECK_EQUAL(barrier->waitFor(-1), mid - 1);

    for (auto l = mid; l <= next; ++l)
    {
        this->m_sequencer->publish(l);
    }

    BOOST_CHECK_EQUAL(barrier->waitFor(-1), next);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldTryNext, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    this->m_sequencer->addGatingSequences({ this->m_gatingSequence });

    for (auto i = 0; i < this->m_bufferSize; i++)
    {
        BOOST_CHECK_NO_THROW(this->m_sequencer->publish(this->m_sequencer->tryNext()));
    }
 
    BOOST_CHECK_THROW(this->m_sequencer->tryNext(), InsufficientCapacityException);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldClaimSpecificSequence, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    std::int64_t sequence = 14;

    this->m_sequencer->claim(sequence);
    this->m_sequencer->publish(sequence);

    BOOST_CHECK_EQUAL(this->m_sequencer->next(), sequence + 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotAllowBulkNextLessThanZero, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_THROW(this->m_sequencer->next(-1), ArgumentException);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotAllowBulkNextOfZero, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_THROW(this->m_sequencer->next(0), ArgumentException);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotAllowBulkTryNextLessThanZero, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_THROW(this->m_sequencer->tryNext(-1), ArgumentException);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ShouldNotAllowBulkTryNextOfZero, TSequencer, Sequencers, SequencerTestFixture< TSequencer >)
{
    BOOST_CHECK_THROW(this->m_sequencer->tryNext(0), ArgumentException);
}

BOOST_AUTO_TEST_SUITE_END()
