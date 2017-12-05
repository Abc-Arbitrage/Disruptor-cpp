#include "stdafx.h"

#include "Disruptor/FixedSequenceGroup.h"
#include "Disruptor/Sequence.h"


using namespace Disruptor;


BOOST_AUTO_TEST_SUITE(FixedSequenceGroupTest)

BOOST_AUTO_TEST_CASE(ShouldReturnMinimumOf2Sequences)
{
    auto sequence1 = std::make_shared< Sequence >(34);
    auto sequence2 = std::make_shared< Sequence >(47);
    FixedSequenceGroup group({ sequence1, sequence2 });

    BOOST_CHECK_EQUAL(group.value(), 34L);
    sequence1->setValue(35);
    BOOST_CHECK_EQUAL(group.value(), 35L);
    sequence1->setValue(48);
    BOOST_CHECK_EQUAL(group.value(), 47L);
}

BOOST_AUTO_TEST_SUITE_END()
