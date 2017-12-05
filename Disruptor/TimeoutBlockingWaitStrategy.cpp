#include "stdafx.h"
#include "TimeoutBlockingWaitStrategy.h"

#include <ostream>

#include <boost/chrono.hpp>

#include "ISequenceBarrier.h"
#include "Sequence.h"
#include "TimeoutException.h"


namespace Disruptor
{

    TimeoutBlockingWaitStrategy::TimeoutBlockingWaitStrategy(ClockConfig::Duration timeout)
        : m_timeout(timeout)
    {
    }

     std::int64_t TimeoutBlockingWaitStrategy::waitFor(std::int64_t sequence,
                                                      const std::shared_ptr< Sequence >& cursor,
                                                      const std::shared_ptr< ISequence >& dependentSequence,
                                                      const std::shared_ptr< ISequenceBarrier >& barrier)
    {
        auto timeSpan = boost::chrono::microseconds(std::chrono::duration_cast< std::chrono::microseconds >(m_timeout).count());

        if (cursor->value() < sequence)
        {
            boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            while (cursor->value() < sequence)
            {
                barrier->checkAlert();

                if (m_conditionVariable.wait_for(uniqueLock, timeSpan) == boost::cv_status::timeout)
                    DISRUPTOR_THROW_TIMEOUT_EXCEPTION();
            }
        }

        std::int64_t availableSequence;
        while ((availableSequence = dependentSequence->value()) < sequence)
        {
            barrier->checkAlert();
        }

        return availableSequence;
    }

    void TimeoutBlockingWaitStrategy::signalAllWhenBlocking()
    {
        boost::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

        m_conditionVariable.notify_all();
    }

    void TimeoutBlockingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "TimeoutBlockingWaitStrategy";
    }

} // namespace Disruptor
