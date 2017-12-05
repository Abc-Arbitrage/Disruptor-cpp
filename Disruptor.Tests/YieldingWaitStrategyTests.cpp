#include "stdafx.h"

#include "Disruptor/YieldingWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(YieldingWaitStrategyTests)

BOOST_AUTO_TEST_CASE(ShouldWaitForValue)
{
    assertWaitForWithDelayOf(50, std::make_shared< YieldingWaitStrategy >());
}

BOOST_AUTO_TEST_SUITE_END()
