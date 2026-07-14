#pragma once

#include <atomic>

#include <boost/thread.hpp>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Variation of the BlockingWaitStrategy that attempts to elide conditional wake-ups when the lock is uncontended.
     * Shows performance improvements on microbenchmarks. However this wait strategy should be considered experimental
     * as the correctness of the lock elision code has not been fully proved.
     */
    class LiteBlockingWaitStrategy : public IWaitStrategy
    {
    public:
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
        boost::recursive_mutex m_gate;
        boost::condition_variable_any m_conditionVariable;
        std::atomic< bool > m_signalNeeded{ false };
    };

} // namespace Disruptor
