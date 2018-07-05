#include "stdafx.h"
#include "SpinWaitWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"
#include "SpinWait.h"


namespace Disruptor
{

    std::int64_t SpinWaitWaitStrategy::waitFor(std::int64_t sequence,
                                               Sequence& /*cursor*/,
                                               ISequence& dependentSequence,
                                               ISequenceBarrier& barrier)
    {
        std::int64_t availableSequence;

        SpinWait spinWait;
        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            barrier.checkAlert();
            spinWait.spinOnce();
        }

        return availableSequence;
    }

    void SpinWaitWaitStrategy::signalAllWhenBlocking()
    {
    }

    void SpinWaitWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "SpinWaitWaitStrategy";
    }

} // namespace Disruptor
