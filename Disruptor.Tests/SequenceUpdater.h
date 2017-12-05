#pragma once

#include <cstdint>

#include "Disruptor/IWaitStrategy.h"
#include "Disruptor/Sequence.h"

#include "Disruptor.TestTools/CountdownEvent.h"


namespace Disruptor
{
namespace Tests
{

    class SequenceUpdater
    {
    public:
        SequenceUpdater(std::int64_t sleepTime, const std::shared_ptr< IWaitStrategy >& waitStrategy);

        void run();

        void waitForStartup();

        std::shared_ptr< Sequence > sequence = std::make_shared< Sequence >();

    private:
        CountdownEvent m_barrier;
        std::int32_t m_sleepTime = 0;
        std::shared_ptr< IWaitStrategy > m_waitStrategy;
    };
    
} // namespace Tests
} // namespace Disruptor
