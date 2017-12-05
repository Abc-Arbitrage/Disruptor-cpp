#include "stdafx.h"
#include "EventCountingQueueProcessor.h"


namespace Disruptor
{
namespace PerfTests
{

    EventCountingQueueProcessor::EventCountingQueueProcessor(const std::shared_ptr< BlockingQueue< std::int64_t > >& blockingQueue,
                                                             const std::vector< std::shared_ptr< PaddedLong > >& counters,
                                                             std::int32_t index)
        : m_running(false)
        , m_blockingQueue(blockingQueue)
        , m_counters(counters)
        , m_index(index)
    {
    }

    void EventCountingQueueProcessor::halt()
    {
        m_running = false;
    }

    void EventCountingQueueProcessor::run()
    {
        m_running = true;
        while (m_running)
        {
            std::int64_t item;
            if (!m_blockingQueue->timedWaitAndPop(item, std::chrono::microseconds(100)))
                continue;

            m_counters[m_index]->value = m_counters[m_index]->value + 1L;
        }
    }

} // namespace PerfTests
} // namespace Disruptor
