#include "stdafx.h"
#include "LiteBlockingWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"


namespace Disruptor
{

    std::int64_t LiteBlockingWaitStrategy::waitFor(std::int64_t sequence,
                                                   Sequence& cursor,
                                                   ISequence& dependentSequence,
                                                   ISequenceBarrier& barrier)
    {
        if (cursor.value() < sequence)
        {
            boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            do
            {
                m_signalNeeded.exchange(true);

                if (cursor.value() >= sequence)
                    break;

                barrier.checkAlert();

                m_conditionVariable.wait(uniqueLock);
            }
            while (cursor.value() < sequence);
        }

        std::int64_t availableSequence;
        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            barrier.checkAlert();
        }

        return availableSequence;
    }

    void LiteBlockingWaitStrategy::signalAllWhenBlocking()
    {
        if (m_signalNeeded.exchange(false))
        {
            boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            m_conditionVariable.notify_all();
        }
    }

    void LiteBlockingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "LiteBlockingWaitStrategy";
    }

} // namespace Disruptor
