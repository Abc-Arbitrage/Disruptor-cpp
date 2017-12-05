#include "stdafx.h"

#include "Disruptor/Sequence.h"
#include "Disruptor/Util.h"


using namespace Disruptor;


BOOST_AUTO_TEST_SUITE(UtilTests)

BOOST_AUTO_TEST_CASE(ShouldReturnNextPowerOfTwo)
{
    auto powerOfTwo = Util::ceilingNextPowerOfTwo(1000);

    BOOST_CHECK_EQUAL(1024, powerOfTwo);
}

BOOST_AUTO_TEST_CASE(ShouldReturnExactPowerOfTwo)
{
    auto powerOfTwo = Util::ceilingNextPowerOfTwo(1024);

    BOOST_CHECK_EQUAL(1024, powerOfTwo);
}

BOOST_AUTO_TEST_CASE(ShouldReturnMinimumSequence)
{
    BOOST_CHECK_EQUAL(4L, Util::getMinimumSequence({ std::make_shared< Sequence >(11), std::make_shared< Sequence >(4), std::make_shared< Sequence >(13) }));
}

BOOST_AUTO_TEST_CASE(ShouldReturnLongMaxWhenNoEventProcessors)
{
    BOOST_CHECK_EQUAL(std::numeric_limits< std::int64_t >::max(), Util::getMinimumSequence({ }));
}

BOOST_AUTO_TEST_SUITE_END()
