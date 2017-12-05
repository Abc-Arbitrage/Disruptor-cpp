#include "stdafx.h"

#include "Disruptor/SleepingWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(SleepingWaitStrategyTests)

BOOST_AUTO_TEST_CASE(ShouldWaitForValue)
{
    assertWaitForWithDelayOf(50, std::make_shared< SleepingWaitStrategy >());
}

BOOST_AUTO_TEST_SUITE_END()
