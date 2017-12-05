#include "stdafx.h"

#include "Disruptor/BusySpinWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(BusySpinWaitStrategyTests)

BOOST_AUTO_TEST_CASE(ShouldWaitForValue)
{
    assertWaitForWithDelayOf(50, std::make_shared< BusySpinWaitStrategy >());
}

BOOST_AUTO_TEST_SUITE_END()
