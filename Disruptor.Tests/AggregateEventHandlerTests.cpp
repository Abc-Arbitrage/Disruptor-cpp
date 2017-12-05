#include "stdafx.h"

#include "Disruptor/AggregateEventHandler.h"

#include "AggregateEventHandlerTestsFixture.h"
#include "SequencerFixture.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(AggregateEventHandlerTests)

BOOST_FIXTURE_TEST_CASE(ShouldCallOnEventInSequence, AggregateEventHandlerTestsFixture)
{
    auto evt = 7;
    const std::int64_t sequence = 3;
    const auto endOfBatch = true;

    EXPECT_CALL(*m_eventHandlerMock1, onEvent(evt, sequence, endOfBatch)).Times(1);
    EXPECT_CALL(*m_eventHandlerMock2, onEvent(evt, sequence, endOfBatch)).Times(1);
    EXPECT_CALL(*m_eventHandlerMock3, onEvent(evt, sequence, endOfBatch)).Times(1);

    m_aggregateEventHandler->onEvent(evt, sequence, endOfBatch);
}

BOOST_FIXTURE_TEST_CASE(ShouldCallOnStartInSequence, AggregateEventHandlerTestsFixture)
{
    EXPECT_CALL(*m_eventHandlerMock1, onStart()).Times(1);
    EXPECT_CALL(*m_eventHandlerMock2, onStart()).Times(1);
    EXPECT_CALL(*m_eventHandlerMock3, onStart()).Times(1);

    m_aggregateEventHandler->onStart();
}

BOOST_FIXTURE_TEST_CASE(ShouldCallOnShutdownInSequence, AggregateEventHandlerTestsFixture)
{
    EXPECT_CALL(*m_eventHandlerMock1, onShutdown()).Times(1);
    EXPECT_CALL(*m_eventHandlerMock2, onShutdown()).Times(1);
    EXPECT_CALL(*m_eventHandlerMock3, onShutdown()).Times(1);

    m_aggregateEventHandler->onShutdown();
}

BOOST_FIXTURE_TEST_CASE(ShouldHandleEmptyListOfEventHandlers, AggregateEventHandlerTestsFixture)
{
    auto newAggregateEventHandler = std::make_shared< AggregateEventHandler< std::int32_t > >();

    auto v = 7;
    BOOST_CHECK_NO_THROW(newAggregateEventHandler->onEvent(v, 0, true));
    BOOST_CHECK_NO_THROW(newAggregateEventHandler->onStart());
    BOOST_CHECK_NO_THROW(newAggregateEventHandler->onShutdown());
}

BOOST_AUTO_TEST_SUITE_END()
