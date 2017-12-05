#include "stdafx.h"

#include "ConsumerRepositoryTestsFixture.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(ConsumerRepositoryTests)

BOOST_FIXTURE_TEST_CASE(ShouldGetBarrierByHandler, ConsumerRepositoryTestsFixture)
{
    m_consumerRepository.add(m_eventProcessor1, m_handler1, m_barrier1);

    BOOST_CHECK_EQUAL(m_consumerRepository.getBarrierFor(m_handler1), m_barrier1);
}

BOOST_FIXTURE_TEST_CASE(ShouldReturnNullForBarrierWhenHandlerIsNotRegistered, ConsumerRepositoryTestsFixture)
{
    BOOST_CHECK(m_consumerRepository.getBarrierFor(m_handler1) == nullptr);
}

BOOST_FIXTURE_TEST_CASE(ShouldGetLastEventProcessorsInChain, ConsumerRepositoryTestsFixture)
{
    m_consumerRepository.add(m_eventProcessor1, m_handler1, m_barrier1);
    m_consumerRepository.add(m_eventProcessor2, m_handler2, m_barrier2);

    m_consumerRepository.unMarkEventProcessorsAsEndOfChain({ m_eventProcessor2->sequence() });

    auto lastEventProcessorsInChain = m_consumerRepository.getLastSequenceInChain(true);

    BOOST_CHECK_EQUAL(lastEventProcessorsInChain.size(), 1u);
    BOOST_CHECK_EQUAL(lastEventProcessorsInChain[0], m_eventProcessor1->sequence());
}

BOOST_FIXTURE_TEST_CASE(ShouldRetrieveEventProcessorForHandler, ConsumerRepositoryTestsFixture)
{
    m_consumerRepository.add(m_eventProcessor1, m_handler1, m_barrier1);

    BOOST_CHECK(m_consumerRepository.getEventProcessorFor(m_handler1) == m_eventProcessor1);
}

BOOST_FIXTURE_TEST_CASE(ShouldThrowExceptionWhenHandlerIsNotRegistered, ConsumerRepositoryTestsFixture)
{
    BOOST_CHECK_THROW(m_consumerRepository.getEventProcessorFor(std::shared_ptr< SleepingEventHandler >()), Disruptor::ArgumentException);
}

BOOST_FIXTURE_TEST_CASE(ShouldIterateAllEventProcessors, ConsumerRepositoryTestsFixture)
{
    m_consumerRepository.add(m_eventProcessor1, m_handler1, m_barrier1);
    m_consumerRepository.add(m_eventProcessor2, m_handler2, m_barrier2);

    auto seen1 = false;
    auto seen2 = false;
    
    for (auto&& testEntryEventProcessorInfo : m_consumerRepository)
    {
        auto eventProcessorInfo = std::dynamic_pointer_cast< EventProcessorInfo< TestEvent > >(testEntryEventProcessorInfo);
        if (!seen1 && eventProcessorInfo->eventProcessor() == m_eventProcessor1 && eventProcessorInfo->handler() == m_handler1)
        {
            seen1 = true;
        }
        else if (!seen2 && eventProcessorInfo->eventProcessor() == m_eventProcessor2 && eventProcessorInfo->handler() == m_handler2)
        {
            seen2 = true;
        }
        else
        {
            BOOST_FAIL("Unexpected eventProcessor info");
        }
    }

    BOOST_CHECK_EQUAL(seen1, true);
    BOOST_CHECK_EQUAL(seen2, true);
}

BOOST_AUTO_TEST_SUITE_END()
