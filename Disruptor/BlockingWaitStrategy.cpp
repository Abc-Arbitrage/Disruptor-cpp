#include "stdafx.h"
#include "BlockingWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"


namespace Disruptor
{

    std::int64_t BlockingWaitStrategy::waitFor(std::int64_t sequence,
                                               Sequence& cursor,
                                               ISequence& dependentSequence,
                                               ISequenceBarrier& barrier)
    {
        if (cursor.value() < sequence)
        {
            boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            while (cursor.value() < sequence)
            {
                barrier.checkAlert();

                m_conditionVariable.wait(uniqueLock);
            }
        }

        std::int64_t availableSequence;
        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            barrier.checkAlert();
        }

        return availableSequence;
    }

    void BlockingWaitStrategy::signalAllWhenBlocking()
    {
        boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

        m_conditionVariable.notify_all();
    }

    void BlockingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "BlockingWaitStrategy";
    }

} // namespace Disruptor
