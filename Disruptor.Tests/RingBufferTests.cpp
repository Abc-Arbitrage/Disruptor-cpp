#include "stdafx.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "RingBufferTestsFixture.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_FIXTURE_TEST_SUITE(RingBufferTests, RingBufferTestsFixture)

BOOST_AUTO_TEST_CASE(ShouldClaimAndGet)
{
    BOOST_CHECK_EQUAL(Sequence::InitialCursorValue, m_ringBuffer->cursor());
    
    auto expectedEvent = StubEvent(2701);

    auto claimSequence = m_ringBuffer->next();
    auto& oldEvent = (*m_ringBuffer)[claimSequence];
    oldEvent.copy(expectedEvent);
    m_ringBuffer->publish(claimSequence);

    auto sequence = m_sequenceBarrier->waitFor(0);
    BOOST_CHECK_EQUAL(0, sequence);

    auto& evt = (*m_ringBuffer)[sequence];
    BOOST_CHECK_EQUAL(expectedEvent, evt);
    
    BOOST_CHECK_EQUAL(0L, m_ringBuffer->cursor());
}

BOOST_AUTO_TEST_CASE(ShouldClaimAndGetInSeparateThread)
{
    auto events = getEvents(0, 0);

    auto expectedEvent = StubEvent(2701);

    auto sequence = m_ringBuffer->next();
    auto& oldEvent = (*m_ringBuffer)[sequence];
    oldEvent.copy(expectedEvent);
    m_ringBuffer->publishEvent(StubEvent::translator(), expectedEvent.value(), expectedEvent.testString());

    BOOST_CHECK_EQUAL(expectedEvent, events.get()[0]);
}

BOOST_AUTO_TEST_CASE(ShouldClaimAndGetMultipleMessages)
{
    auto numEvents = m_ringBuffer->bufferSize();
    for (auto i = 0; i < numEvents; ++i)
    {
        m_ringBuffer->publishEvent(StubEvent::translator(), i, std::string());
    }

    auto expectedSequence = numEvents - 1;
    auto available = m_sequenceBarrier->waitFor(expectedSequence);
    BOOST_CHECK_EQUAL(expectedSequence, available);

    for (auto i = 0; i < numEvents; ++i)
    {
        BOOST_CHECK_EQUAL(i, (*m_ringBuffer)[i].value());
    }
}

BOOST_AUTO_TEST_CASE(ShouldWrap)
{
    auto numEvents = m_ringBuffer->bufferSize();
    auto offset = 1000;
    for (auto i = 0; i < numEvents + offset; ++i)
    {
        m_ringBuffer->publishEvent(StubEvent::translator(), i, std::string());
    }

    auto expectedSequence = numEvents + offset - 1;
    auto available = m_sequenceBarrier->waitFor(expectedSequence);
    BOOST_CHECK_EQUAL(expectedSequence, available);

    for (auto i = offset; i < numEvents + offset; ++i)
    {
        BOOST_CHECK_EQUAL(i, (*m_ringBuffer)[i].value());
    }
}

BOOST_AUTO_TEST_CASE(ShouldPreventWrapping)
{
    auto sequence = std::make_shared< Sequence >(Sequence::InitialCursorValue);
    auto ringBuffer = RingBuffer< StubEvent >::createMultiProducer([] { return StubEvent(-1); }, 4);
    ringBuffer->addGatingSequences({ sequence });

    ringBuffer->publishEvent(StubEvent::translator(), 0, std::string("0"));
    ringBuffer->publishEvent(StubEvent::translator(), 1, std::string("1"));
    ringBuffer->publishEvent(StubEvent::translator(), 2, std::string("2"));
    ringBuffer->publishEvent(StubEvent::translator(), 3, std::string("3"));

    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvent(StubEvent::translator(), 3, std::string("3")), false);
}

BOOST_AUTO_TEST_CASE(ShouldThrowExceptionIfBufferIsFull)
{
    m_ringBuffer->addGatingSequences({ std::make_shared< Sequence >(m_ringBuffer->bufferSize()) });

    try
    {
        for (auto i = 0; i < m_ringBuffer->bufferSize(); ++i)
        {
            m_ringBuffer->publish(m_ringBuffer->tryNext());
        }
    }
    catch (std::exception&)
    {
        throw std::logic_error("Should not have thrown exception");
    }

    try
    {
        m_ringBuffer->tryNext();
        throw std::logic_error("Exception should have been thrown");
    }
    catch (InsufficientCapacityException&)
    {
    }
}

BOOST_AUTO_TEST_CASE(ShouldPreventProducersOvertakingEventProcessorsWrapPoint)
{
    auto ringBufferSize = 4;
    ManualResetEvent mre(false);
    auto producerComplete = false;
    auto ringBuffer = std::make_shared< RingBuffer< StubEvent > >([] { return StubEvent(-1); }, ringBufferSize);
    auto processor = std::make_shared< TestEventProcessor >(ringBuffer->newBarrier());
    ringBuffer->addGatingSequences({ processor->sequence() });

    std::thread thread([&]
    {
        for (auto i = 0; i <= ringBufferSize; i++) // produce 5 events
        {
            auto sequence = ringBuffer->next();
            auto& evt = (*ringBuffer)[sequence];
            evt.value(i);
            ringBuffer->publish(sequence);
        
            if (i == 3) // unblock main thread after 4th eventData published
            {
                mre.set();
            }
        }
        
        producerComplete = true;
    });

    mre.waitOne();

    BOOST_CHECK_EQUAL(ringBuffer->cursor(), ringBufferSize - 1);
    BOOST_CHECK_EQUAL(producerComplete, false);

    processor->run();

    if (thread.joinable())
        thread.join();

    BOOST_CHECK_EQUAL(producerComplete, true);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEvent)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();

    ringBuffer->publishEvent(translator);
    ringBuffer->tryPublishEvent(translator);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents(0, 1), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventOneArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();
    
    ringBuffer->publishEvent(translator, std::string("Foo"));
    ringBuffer->tryPublishEvent(translator, std::string("Foo"));

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents(std::string("Foo-0"), std::string("Foo-1")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventTwoArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    ringBuffer->publishEvent(translator, std::string("Foo"), std::string("Bar"));
    ringBuffer->tryPublishEvent(translator, std::string("Foo"), std::string("Bar"));

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents(std::string("FooBar-0"), std::string("FooBar-1")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventThreeArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    ringBuffer->publishEvent(translator, std::string("Foo"), std::string("Bar"), std::string("Baz"));
    ringBuffer->tryPublishEvent(translator, std::string("Foo"), std::string("Bar"), std::string("Baz"));

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents(std::string("FooBarBaz-0"), std::string("FooBarBaz-1")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEvents)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto eventTranslator = std::make_shared< NoArgEventTranslator >();
    auto translators = { eventTranslator, eventTranslator };

    ringBuffer->publishEvents(translators);
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translators), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(0, 1, 2, 3), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsIfBatchIsLargerThanRingBuffer)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto eventTranslator = std::make_shared< NoArgEventTranslator >();
    auto translators = { eventTranslator, eventTranslator, eventTranslator, eventTranslator, eventTranslator };

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translators), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsWithBatchSizeOfOne)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto eventTranslator = std::make_shared< NoArgEventTranslator >();
    auto translators = { eventTranslator, eventTranslator, eventTranslator };

    ringBuffer->publishEvents(translators, 0, 1);
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translators, 0, 1), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(0, 1, boost::any(), boost::any()), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsWithinBatch)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto eventTranslator = std::make_shared< NoArgEventTranslator >();
    auto translators = { eventTranslator, eventTranslator, eventTranslator };

    ringBuffer->publishEvents(translators, 1, 2);
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translators, 1, 2), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(0, 1, 2, 3), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsOneArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    ringBuffer->publishEvents(translator, { std::string("Foo"), std::string("Foo") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, { std::string("Foo"), std::string("Foo") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo-0"), std::string("Foo-1"), std::string("Foo-2"), std::string("Foo-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsOneArgIfBatchIsLargerThanRingBuffer)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, { std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsOneArgBatchSizeOfOne)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    ringBuffer->publishEvents(translator, 0, 1, { std::string("Foo"), std::string("Foo") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 0, 1, { std::string("Foo"), std::string("Foo") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo-0"), std::string("Foo-1"), boost::any(), boost::any()), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsOneArgWithinBatch)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    ringBuffer->publishEvents(translator, 1, 2, { std::string("Foo"), std::string("Foo"), std::string("Foo") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 1, 2, { std::string("Foo"), std::string("Foo"), std::string("Foo") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo-0"), std::string("Foo-1"), std::string("Foo-2"), std::string("Foo-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsTwoArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    ringBuffer->publishEvents(translator, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("FooBar-0"), std::string("FooBar-1"), std::string("FooBar-2"), std::string("FooBar-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsITwoArgIfBatchSizeIsBiggerThanRingBuffer)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents
    (
        translator,
        { std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo") },
        { std::string("Bar"), std::string("Bar"), std::string("Bar"), std::string("Bar"), std::string("Bar") }
    ), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsTwoArgWithBatchSizeOfOne)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    ringBuffer->publishEvents(translator, 0, 1, { std::string("Foo0"), std::string("Foo1") }, { std::string("Bar0"), std::string("Bar1") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 0, 1, { std::string("Foo2"), std::string("Foo3") }, { std::string("Bar2"), std::string("Bar3") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo0Bar0-0"), std::string("Foo2Bar2-1"), boost::any(), boost::any()), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsTwoArgWithinBatch)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    ringBuffer->publishEvents(translator, 1, 2, { std::string("Foo0"), std::string("Foo1"), std::string("Foo2") }, { std::string("Bar0"), std::string("Bar1"), std::string("Bar2") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 1, 2, { std::string("Foo3"), std::string("Foo4"), std::string("Foo5") }, { std::string("Bar3"), std::string("Bar4"), std::string("Bar5") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo1Bar1-0"), std::string("Foo2Bar2-1"), std::string("Foo4Bar4-2"), std::string("Foo5Bar5-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsThreeArg)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    ringBuffer->publishEvents(translator, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }, { std::string("Baz"), std::string("Baz") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }, { std::string("Baz"), std::string("Baz") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("FooBarBaz-0"), std::string("FooBarBaz-1"), std::string("FooBarBaz-2"), std::string("FooBarBaz-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsThreeArgIfBatchIsLargerThanRingBuffer)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents
    (
        translator,
        { std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo"), std::string("Foo") },
        { std::string("Bar"), std::string("Bar"), std::string("Bar"), std::string("Bar"), std::string("Bar") },
        { std::string("Baz"), std::string("Baz"), std::string("Baz"), std::string("Baz"), std::string("Baz") }
    ), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsThreeArgBatchSizeOfOne)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    ringBuffer->publishEvents(translator, 0, 1, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }, { std::string("Baz"), std::string("Baz") });
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 0, 1, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }, { std::string("Baz"), std::string("Baz") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("FooBarBaz-0"), std::string("FooBarBaz-1"), boost::any(), boost::any()), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldPublishEventsThreeArgWithinBatch)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    ringBuffer->publishEvents(translator, 1, 2, { std::string("Foo0"), std::string("Foo1"), std::string("Foo2") },
                                                { std::string("Bar0"), std::string("Bar1"), std::string("Bar2") },
                                                { std::string("Baz0"), std::string("Baz1"), std::string("Baz2") });
    
    BOOST_CHECK_EQUAL(ringBuffer->tryPublishEvents(translator, 1, 2, { std::string("Foo3"), std::string("Foo4"), std::string("Foo5") },
                                                                     { std::string("Bar3"), std::string("Bar4"), std::string("Bar5") },
                                                                     { std::string("Baz3"), std::string("Baz4"), std::string("Baz5") }), true);

    BOOST_CHECK_EQUAL(m_ringBufferWithEvents4(std::string("Foo1Bar1Baz1-0"), std::string("Foo2Bar2Baz2-1"), std::string("Foo4Bar4Baz4-2"), std::string("Foo5Bar5Baz5-3")), *ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsWhenBatchSizeIs0)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translators, 1, 0), ArgumentException);
    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translators, 1, 0), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translators, 1, 3), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translators, 1, 3), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translators, 1, -1), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translators, 1, -1), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translators, -1, 2), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< NoArgEventTranslator >();
    auto translators = { translator, translator, translator, translator };

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translators, -1, 2), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsOneArgWhenBatchSizeIs0)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") }), ArgumentException);
    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsOneArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsOneArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsOneArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsOneArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsOneArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsOneArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< OneArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsTwoArgWhenBatchSizeIs0)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);
    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsTwoArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsTwoArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsTwoArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsTwoArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsTwoArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsTwoArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< TwoArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") }, { std::string("Bar"), std::string("Bar") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsThreeArgWhenBatchSizeIs0)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") },
                                                                  { std::string("Bar"), std::string("Bar") },
                                                                  { std::string("Baz"), std::string("Baz") }), ArgumentException);

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 0, { std::string("Foo"), std::string("Foo") },
                                                                     { std::string("Bar"), std::string("Bar") },
                                                                     { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsThreeArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") },
                                                                  { std::string("Bar"), std::string("Bar") },
                                                                  { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsThreeArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") },
                                                                   { std::string("Bar"), std::string("Bar") },
                                                                   { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotPublishEventsThreeArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->publishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") },
                                                                   { std::string("Bar"), std::string("Bar") },
                                                                   { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsThreeArgWhenBatchExtendsPastEndOfArray)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, 3, { std::string("Foo"), std::string("Foo") },
                                                                     { std::string("Bar"), std::string("Bar") },
                                                                     { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsThreeArgWhenBatchSizeIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, 1, -1, { std::string("Foo"), std::string("Foo") },
                                                                      { std::string("Bar"), std::string("Bar") },
                                                                      { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldNotTryPublishEventsThreeArgWhenBatchStartsAtIsNegative)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 4);
    auto translator = std::make_shared< ThreeArgEventTranslator >();

    BOOST_CHECK_THROW(ringBuffer->tryPublishEvents(translator, -1, 2, { std::string("Foo"), std::string("Foo") },
                                                                      { std::string("Bar"), std::string("Bar") },
                                                                      { std::string("Baz"), std::string("Baz") }), ArgumentException);

    assertEmptyRingBuffer(*ringBuffer);
}

BOOST_AUTO_TEST_CASE(ShouldAddAndRemoveSequences)
{
    auto ringBuffer = RingBuffer< boost::any >::createSingleProducer([] { return boost::any(); }, 16);
    auto sequenceThree = std::make_shared< Sequence >(-1);
    auto sequenceSeven = std::make_shared< Sequence >(-1);

    ringBuffer->addGatingSequences({ sequenceThree, sequenceSeven });

    for (auto i = 0; i < 10; i++)
    {
        ringBuffer->publish(ringBuffer->next());
    }
    
    sequenceThree->setValue(3);
    sequenceSeven->setValue(7);
    
    BOOST_CHECK_EQUAL(ringBuffer->getMinimumGatingSequence(), 3L);
    BOOST_CHECK_EQUAL(ringBuffer->removeGatingSequence(sequenceThree), true);
    BOOST_CHECK_EQUAL(ringBuffer->getMinimumGatingSequence(), 7L);
}

BOOST_AUTO_TEST_CASE(ShouldHandleResetToAndNotWrapUnecessarilySingleProducer)
{
    assertHandleResetAndNotWrap(RingBuffer< StubEvent >::createSingleProducer(StubEvent::eventFactory(), 4));
}

BOOST_AUTO_TEST_CASE(ShouldHandleResetToAndNotWrapUnecessarilyMultiProducer)
{
    assertHandleResetAndNotWrap(RingBuffer< StubEvent >::createMultiProducer(StubEvent::eventFactory(), 4));
}

BOOST_AUTO_TEST_SUITE_END()
