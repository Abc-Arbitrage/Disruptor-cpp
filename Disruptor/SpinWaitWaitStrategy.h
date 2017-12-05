#pragma once

#include <boost/thread.hpp>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /// <summary>
    /// Spin strategy that uses a <see cref="SpinWait"/> for <see cref="IEventProcessor"/>s waiting on a barrier.
    /// <p>
    /// This strategy is a good compromise between performance and CPU resource.
    /// Latency spikes can occur after quiet periods.
    /// </p>
    /// </summary>
    class SpinWaitWaitStrategy : public IWaitStrategy
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
