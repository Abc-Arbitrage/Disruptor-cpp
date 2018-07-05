#pragma once

#include <boost/thread.hpp>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Blocking strategy that uses a lock and condition variable for IEventProcessor's waiting on a barrier.
     * This strategy should be used when performance and low-latency are not as important as CPU resource.
     */ 
    class BlockingWaitStrategy : public IWaitStrategy
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
    };

} // namespace Disruptor
