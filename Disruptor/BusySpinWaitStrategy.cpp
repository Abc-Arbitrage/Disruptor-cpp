#include "stdafx.h"
#include "BusySpinWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"


namespace Disruptor
{

    std::int64_t BusySpinWaitStrategy::waitFor(std::int64_t sequence,
                                               Sequence& /*cursor*/,
                                               ISequence& dependentSequence,
                                               ISequenceBarrier& barrier)
    {
        std::int64_t availableSequence;

        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            barrier.checkAlert();
        }

        return availableSequence;
    }

    void BusySpinWaitStrategy::signalAllWhenBlocking()
    {
    }

    void BusySpinWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "BusySpinWaitStrategy";
    }

} // namespace Disruptor
