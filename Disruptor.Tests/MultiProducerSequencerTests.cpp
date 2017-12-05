#include "stdafx.h"

#include "Disruptor/BlockingWaitStrategy.h"
#include "Disruptor/MultiProducerSequencer.h"


using namespace Disruptor;


BOOST_AUTO_TEST_SUITE(MultiProducerSequencerTests)

BOOST_AUTO_TEST_CASE(ShouldOnlyAllowMessagesToBeAvailableIfSpecificallyPublished)
{
    auto waitingStrategy = std::make_shared< BlockingWaitStrategy >();
    auto publisher = std::make_shared< MultiProducerSequencer< int > >(1024, waitingStrategy);

    publisher->publish(3);
    publisher->publish(5);

    BOOST_CHECK_EQUAL(publisher->isAvailable(0), false);
    BOOST_CHECK_EQUAL(publisher->isAvailable(1), false);
    BOOST_CHECK_EQUAL(publisher->isAvailable(2), false);
    BOOST_CHECK_EQUAL(publisher->isAvailable(3), true );
    BOOST_CHECK_EQUAL(publisher->isAvailable(4), false);
    BOOST_CHECK_EQUAL(publisher->isAvailable(5), true );
    BOOST_CHECK_EQUAL(publisher->isAvailable(6), false);
}

BOOST_AUTO_TEST_SUITE_END()
