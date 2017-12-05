#pragma once

#include <mutex>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /// <summary>
    /// Busy Spin strategy that uses a busy spin loop for <see cref="IEventProcessor"/>s waiting on a barrier.
    /// 
    /// This strategy will use CPU resource to avoid syscalls which can introduce latency jitter.  It is best
    /// used when threads can be bound to specific CPU cores.
    /// </summary>
    class BusySpinWaitStrategy : public IWaitStrategy
    {
    public:
        /// <summary>
        /// <see cref="IWaitStrategy.WaitFor"/>
        /// </summary>
        std::int64_t waitFor(std::int64_t sequence,
                             const std::shared_ptr< Sequence >& cursor,
                             const std::shared_ptr< ISequence >& dependentSequence,
                             const std::shared_ptr< ISequenceBarrier >& barrier) override;

        /// <summary>
        /// <see cref="IWaitStrategy.SignalAllWhenBlocking"/>
        /// </summary>
        void signalAllWhenBlocking() override;

        void writeDescriptionTo(std::ostream& stream) const override;
    };

} // namespace Disruptor
