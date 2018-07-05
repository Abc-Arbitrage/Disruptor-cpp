#include "stdafx.h"
#include "YieldingWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"


namespace Disruptor
{

    std::int64_t YieldingWaitStrategy::waitFor(std::int64_t sequence,
                                               Sequence& /*cursor*/,
                                               ISequence& dependentSequence,
                                               ISequenceBarrier& barrier)
    {
        std::int64_t availableSequence;
        auto counter = m_spinTries;

        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            counter = applyWaitMethod(barrier, counter);
        }

        return availableSequence;
    }

    void YieldingWaitStrategy::signalAllWhenBlocking()
    {
    }

    std::int32_t YieldingWaitStrategy::applyWaitMethod(ISequenceBarrier& barrier, std::int32_t counter)
    {
        barrier.checkAlert();

        if (counter == 0)
        {
            std::this_thread::yield();
        }
        else
        {
            --counter;
        }

        return counter;
    }

    void YieldingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "YieldingWaitStrategy";
    }

} // namespace Disruptor
