#pragma once

#include <boost/thread.hpp>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Spin strategy that uses a SpinWait for IEventProcessors waiting on a barrier. 
     * This strategy is a good compromise between performance and CPU resource. Latency spikes can occur after quiet periods.
     */ 
    class SpinWaitWaitStrategy : public IWaitStrategy
    {
    public:
        /**
         * \see IWaitStrategy.waitFor()
         */ 
        std::int64_t waitFor(std::int64_t sequence,
                             Sequence& cursor,
                             ISequence& dependentSequence,
                             ISequenceBarrier& barrier) override;

        /**
         * \see IWaitStrategy.signalAllWhenBlocking()
         */ 
        void signalAllWhenBlocking() override;

        void writeDescriptionTo(std::ostream& stream) const override;
    };

} // namespace Disruptor
