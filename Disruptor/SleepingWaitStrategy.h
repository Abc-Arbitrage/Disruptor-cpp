#pragma once

#include <cstdint>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /// <summary>
    /// Sleeping strategy that initially spins, then uses a Thread.Yield(), and
    /// eventually sleep(<code>Thread.Sleep(0)</code>) for the minimum
    /// number of nanos the OS and JVM will allow while the
    /// <see cref="IEventProcessor"/>s are waiting on a barrier.
    /// <p>
    /// This strategy is a good compromise between performance and CPU resource.
    /// Latency spikes can occur after quiet periods.
    /// </p>
    /// </summary>
    class SleepingWaitStrategy : public IWaitStrategy
    {
    public:
        explicit SleepingWaitStrategy(std::int32_t retries = m_defaultRetries);

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

    private:
        static std::int32_t applyWaitMethod(ISequenceBarrier& barrier, std::int32_t counter);

        static const std::int32_t m_defaultRetries = 200;
        std::int32_t m_retries = 0;
    };

} // namespace Disruptor
