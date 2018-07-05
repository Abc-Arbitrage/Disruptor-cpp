#pragma once

#include "Disruptor/IWaitStrategy.h"

namespace Disruptor
{

    /**
     * Yielding strategy that uses a Thread.Yield() for IEventProcessors waiting on a barrier after an initially spinning.
     * This strategy is a good compromise between performance and CPU resource without incurring significant latency spikes.
     */ 
    class YieldingWaitStrategy : public IWaitStrategy
    {
    public:
        /**
         * Wait for the given sequence to be available. 
         * This strategy is a good compromise between performance and CPU resource without incurring significant latency spikes.
         *
         * \param sequence sequence to be waited on.
         * \param cursor Ring buffer cursor on which to wait.
         * \param dependentSequence dependents further back the chain that must advance first
         * \param barrier barrier the IEventProcessor is waiting on.
         * \returns the sequence that is available which may be greater than the requested sequence.
         */ 
        std::int64_t waitFor(std::int64_t sequence,
                             Sequence& cursor,
                             ISequence& dependentSequence,
                             ISequenceBarrier& barrier) override;

        /**
         * Signal those IEventProcessor waiting that the cursor has advanced.
         */ 
        void signalAllWhenBlocking() override;

        static std::int32_t applyWaitMethod(ISequenceBarrier& barrier, std::int32_t counter);

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        const std::int32_t m_spinTries = 100;
    };

} // namespace Disruptor
