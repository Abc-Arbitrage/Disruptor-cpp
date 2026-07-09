#pragma once

#include <memory>

#include "Disruptor/ClockConfig.h"
#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    /**
     * Phased wait strategy for waiting IEventProcessors on a barrier.
     * Spins, then yields, then waits using the configured fallback IWaitStrategy.
     * This strategy can be used when throughput and low-latency are not as important as CPU resource.
     */
    class PhasedBackoffWaitStrategy : public IWaitStrategy
    {
    public:
        PhasedBackoffWaitStrategy(ClockConfig::Duration spinTimeout,
                                  ClockConfig::Duration yieldTimeout,
                                  const std::shared_ptr< IWaitStrategy >& fallbackStrategy);

        /**
         * Construct PhasedBackoffWaitStrategy with fallback to BlockingWaitStrategy.
         *
         * \param spinTimeout The maximum time in to busy spin for.
         * \param yieldTimeout The maximum time in to yield for.
         * \returns The constructed wait strategy.
         */
        static std::shared_ptr< PhasedBackoffWaitStrategy > withLock(ClockConfig::Duration spinTimeout, ClockConfig::Duration yieldTimeout);

        /**
         * Construct PhasedBackoffWaitStrategy with fallback to LiteBlockingWaitStrategy.
         *
         * \param spinTimeout The maximum time in to busy spin for.
         * \param yieldTimeout The maximum time in to yield for.
         * \returns The constructed wait strategy.
         */
        static std::shared_ptr< PhasedBackoffWaitStrategy > withLiteLock(ClockConfig::Duration spinTimeout, ClockConfig::Duration yieldTimeout);

        /**
         * Construct PhasedBackoffWaitStrategy with fallback to SleepingWaitStrategy.
         *
         * \param spinTimeout The maximum time in to busy spin for.
         * \param yieldTimeout The maximum time in to yield for.
         * \returns The constructed wait strategy.
         */
        static std::shared_ptr< PhasedBackoffWaitStrategy > withSleep(ClockConfig::Duration spinTimeout, ClockConfig::Duration yieldTimeout);

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
        const std::int32_t m_spinTries = 10000;
        ClockConfig::Duration m_spinTimeout;
        ClockConfig::Duration m_yieldTimeout;
        std::shared_ptr< IWaitStrategy > m_fallbackStrategy;
    };

} // namespace Disruptor
