#pragma once

#include <boost/thread.hpp>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /// <summary>
    /// Blocking strategy that uses a lock and condition variable for <see cref="IEventProcessor"/>s waiting on a barrier.
    /// 
    /// This strategy should be used when performance and low-latency are not as important as CPU resource.
    /// </summary>
    class BlockingWaitStrategy : public IWaitStrategy
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

    private:
        boost::recursive_mutex m_gate;
        boost::condition_variable_any m_conditionVariable;
    };

} // namespace Disruptor
