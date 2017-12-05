#pragma once

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /// <summary>
    /// Yielding strategy that uses a Thread.Yield() for <see cref="IEventProcessor"/>s waiting on a barrier
    /// after an initially spinning.
    /// 
    /// This strategy is a good compromise between performance and CPU resource without incurring significant latency spikes.
    /// </summary>
    class YieldingWaitStrategy : public IWaitStrategy
    {
    public:
        /// <summary>
        /// Wait for the given sequence to be available
        /// <para>This strategy is a good compromise between performance and CPU resource without incurring significant latency spikes.</para>
        /// </summary>
        /// <param name="sequence">sequence to be waited on.</param>
        /// <param name="cursor">Ring buffer cursor on which to wait.</param>
        /// <param name="dependentSequence">dependents further back the chain that must advance first</param>
        /// <param name="barrier">barrier the <see cref="IEventProcessor"/> is waiting on.</param>
        /// <returns>the sequence that is available which may be greater than the requested sequence.</returns>
        std::int64_t waitFor(std::int64_t sequence,
                             const std::shared_ptr< Sequence >& cursor,
                             const std::shared_ptr< ISequence >& dependentSequence,
                             const std::shared_ptr< ISequenceBarrier >& barrier) override;

        /// <summary>
        /// Signal those <see cref="IEventProcessor"/> waiting that the cursor has advanced.
        /// </summary>
        void signalAllWhenBlocking() override;

        static std::int32_t applyWaitMethod(ISequenceBarrier& barrier, std::int32_t counter);

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        const std::int32_t m_spinTries = 100;
    };

} // namespace Disruptor
