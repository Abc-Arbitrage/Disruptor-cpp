#pragma once

#include <atomic>

#include <boost/thread.hpp>

#include "Disruptor/ClockConfig.h"
#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Variation of the TimeoutBlockingWaitStrategy that attempts to elide conditional wake-ups
     * when the lock is uncontended.
     */
    class LiteTimeoutBlockingWaitStrategy : public IWaitStrategy
    {
    public:
        explicit LiteTimeoutBlockingWaitStrategy(ClockConfig::Duration timeout);

        /**
         * \see IWaitStrategy::waitFor
         */
        std::int64_t waitFor(std::int64_t sequence,
                             Sequence& cursor,
                             ISequence& dependentSequence,
                             ISequenceBarrier& barrier) override;

        /**
         * \see IWaitStrategy::signalAllWhenBlocking
         */
        void signalAllWhenBlocking() override;

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        ClockConfig::Duration m_timeout;
        boost::recursive_mutex m_gate;
        boost::condition_variable_any m_conditionVariable;
        std::atomic< bool > m_signalNeeded{ false };
    };

} // namespace Disruptor
