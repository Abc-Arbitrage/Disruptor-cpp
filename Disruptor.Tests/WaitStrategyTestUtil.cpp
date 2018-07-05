#include "stdafx.h"
#include "WaitStrategyTestUtil.h"

#include "Disruptor/IWaitStrategy.h"
#include "Disruptor/Sequence.h"

#include "DummySequenceBarrier.h"
#include "SequenceUpdater.h"


namespace Disruptor
{
namespace Tests
{

    void assertWaitForWithDelayOf(std::int64_t sleepTimeMillis, const std::shared_ptr< IWaitStrategy >& waitStrategy)
    {
        auto sequencerUpdater = std::make_shared< SequenceUpdater >(sleepTimeMillis, waitStrategy);
        std::thread([=] { sequencerUpdater->run(); }).detach();
        sequencerUpdater->waitForStartup();
        auto cursor = std::make_shared< Sequence >(0);
        auto barrier = std::make_shared< DummySequenceBarrier >();
        auto sequence = waitStrategy->waitFor(0, *cursor, *sequencerUpdater->sequence, *barrier);

        BOOST_CHECK_EQUAL(sequence, 0L);
    }
    
} // namespace Tests
} // namespace Disruptor
