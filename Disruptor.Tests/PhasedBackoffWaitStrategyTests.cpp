#include "stdafx.h"

#include "Disruptor/PhasedBackoffWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(PhasedBackoffWaitStrategyTests)

BOOST_AUTO_TEST_CASE(ShouldHandleImmediateSequenceChange)
{
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

BOOST_AUTO_TEST_CASE(ShouldHandleSequenceChangeWithOneMillisecondDelay)
{
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

BOOST_AUTO_TEST_CASE(ShouldHandleSequenceChangeWithTwoMillisecondDelay)
{
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

BOOST_AUTO_TEST_CASE(ShouldHandleSequenceChangeWithTenMillisecondDelay)
{
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

BOOST_AUTO_TEST_SUITE_END()
