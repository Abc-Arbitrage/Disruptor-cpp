#pragma once

#include <boost/thread.hpp>

#include "Disruptor/ClockConfig.h"
#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{

    class TimeoutBlockingWaitStrategy : public IWaitStrategy
    {
    public:
        explicit TimeoutBlockingWaitStrategy(ClockConfig::Duration timeout);

        /**
         * \see IWaitStrategy.waitFor
         */ 
        std::int64_t waitFor(std::int64_t sequence,
                             Sequence& cursor,
                             ISequence& dependentSequence,
                             ISequenceBarrier& barrier) override;

        /**
         * \see IWaitStrategy.signalAllWhenBlocking
         */ 
        void signalAllWhenBlocking() override;

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        ClockConfig::Duration m_timeout;
        boost::recursive_mutex m_gate;
        boost::condition_variable_any m_conditionVariable;
    };

} // namespace Disruptor
