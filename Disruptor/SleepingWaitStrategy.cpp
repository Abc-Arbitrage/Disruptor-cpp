#include "stdafx.h"
#include "SleepingWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"


namespace Disruptor
{

    SleepingWaitStrategy::SleepingWaitStrategy(std::int32_t retries)
        : m_retries(retries)
    {
    }

    std::int64_t SleepingWaitStrategy::waitFor(std::int64_t sequence,
                                               Sequence& /*cursor*/,
                                               ISequence& dependentSequence,
                                               ISequenceBarrier& barrier)
    {
        std::int64_t availableSequence;
        auto counter = m_retries;

        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            counter = applyWaitMethod(barrier, counter);
        }

        return availableSequence;
    }

    void SleepingWaitStrategy::signalAllWhenBlocking()
    {
    }

    void SleepingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "SleepingWaitStrategy";
    }

    std::int32_t SleepingWaitStrategy::applyWaitMethod(ISequenceBarrier& barrier, std::int32_t counter)
    {
        barrier.checkAlert();

        if (counter > 100)
        {
            --counter;
        }
        else if (counter > 0)
        {
            --counter;
            std::this_thread::yield();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }

        return counter;
    }

} // namespace Disruptor
