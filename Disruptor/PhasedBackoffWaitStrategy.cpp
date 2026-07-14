#include "stdafx.h"
#include "PhasedBackoffWaitStrategy.h"

#include <ostream>
#include <thread>

#include "ArgumentNullException.h"
#include "BlockingWaitStrategy.h"
#include "LiteBlockingWaitStrategy.h"
#include "SleepingWaitStrategy.h"


namespace Disruptor
{

    PhasedBackoffWaitStrategy::PhasedBackoffWaitStrategy(ClockConfig::Duration spinTimeout,
                                                         ClockConfig::Duration yieldTimeout,
                                                         const std::shared_ptr< IWaitStrategy >& fallbackStrategy)
        : m_spinTimeout(spinTimeout)
        , m_yieldTimeout(spinTimeout + yieldTimeout)
        , m_fallbackStrategy(fallbackStrategy)
    {
        if (fallbackStrategy == nullptr)
            DISRUPTOR_THROW_ARGUMENT_NULL_EXCEPTION(fallbackStrategy);
    }

    std::shared_ptr< PhasedBackoffWaitStrategy > PhasedBackoffWaitStrategy::withLock(ClockConfig::Duration spinTimeout,
                                                                                     ClockConfig::Duration yieldTimeout)
    {
        return std::make_shared< PhasedBackoffWaitStrategy >(spinTimeout, yieldTimeout, std::make_shared< BlockingWaitStrategy >());
    }

    std::shared_ptr< PhasedBackoffWaitStrategy > PhasedBackoffWaitStrategy::withLiteLock(ClockConfig::Duration spinTimeout,
                                                                                         ClockConfig::Duration yieldTimeout)
    {
        return std::make_shared< PhasedBackoffWaitStrategy >(spinTimeout, yieldTimeout, std::make_shared< LiteBlockingWaitStrategy >());
    }

    std::shared_ptr< PhasedBackoffWaitStrategy > PhasedBackoffWaitStrategy::withSleep(ClockConfig::Duration spinTimeout,
                                                                                      ClockConfig::Duration yieldTimeout)
    {
        return std::make_shared< PhasedBackoffWaitStrategy >(spinTimeout, yieldTimeout, std::make_shared< SleepingWaitStrategy >(0));
    }

    std::int64_t PhasedBackoffWaitStrategy::waitFor(std::int64_t sequence,
                                                    Sequence& cursor,
                                                    ISequence& dependentSequence,
                                                    ISequenceBarrier& barrier)
    {
        std::int64_t availableSequence;
        ClockConfig::TimePoint startTime;
        auto startTimeSet = false;
        auto counter = m_spinTries;

        do
        {
            if ((availableSequence = dependentSequence.value()) >= sequence)
                return availableSequence;

            if (--counter == 0)
            {
                if (!startTimeSet)
                {
                    startTime = ClockConfig::Clock::now();
                    startTimeSet = true;
                }
                else
                {
                    auto timeDelta = ClockConfig::Clock::now() - startTime;
                    if (timeDelta > m_yieldTimeout)
                        return m_fallbackStrategy->waitFor(sequence, cursor, dependentSequence, barrier);

                    if (timeDelta > m_spinTimeout)
                        std::this_thread::yield();
                }

                counter = m_spinTries;
            }
        }
        while (true);
    }

    void PhasedBackoffWaitStrategy::signalAllWhenBlocking()
    {
        m_fallbackStrategy->signalAllWhenBlocking();
    }

    void PhasedBackoffWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "PhasedBackoffWaitStrategy (fallback: ";
        m_fallbackStrategy->writeDescriptionTo(stream);
        stream << ")";
    }

} // namespace Disruptor
