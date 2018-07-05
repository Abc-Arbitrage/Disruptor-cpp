#pragma once

#include <mutex>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Busy Spin strategy that uses a busy spin loop for IEventProcessor's waiting on a barrier.
     * This strategy will use CPU resource to avoid syscalls which can introduce latency jitter.  It is best used when threads can be bound to specific CPU cores.
     */ 
    class BusySpinWaitStrategy : public IWaitStrategy
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
    };

} // namespace Disruptor
