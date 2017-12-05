#pragma once

#include "Disruptor/BlockingQueue.h"
#include "Disruptor/IWorkHandler.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/WorkerPool.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.PerfTests/EventCountingQueueProcessor.h"
#include "Disruptor.PerfTests/EventCountingWorkHandler.h"
#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/PaddedLong.h"


namespace Disruptor
{
namespace PerfTests
{

    class OneToThreeWorkerPoolThroughputTest : public IThroughputTest
    {
    public:
        OneToThreeWorkerPoolThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        void resetCounters();
        std::int64_t sumCounters();

        const std::int32_t m_numWorkers = 3;
        const std::int32_t m_bufferSize = 1024 * 8;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        std::vector< std::shared_ptr< PaddedLong > > m_counters = std::vector< std::shared_ptr< PaddedLong > >(m_numWorkers);

        std::shared_ptr< BlockingQueue< std::int64_t > > m_blockingQueue = std::make_shared< BlockingQueue< std::int64_t > >();
        std::vector< std::shared_ptr< EventCountingQueueProcessor > > m_queueWorkers = std::vector< std::shared_ptr< EventCountingQueueProcessor > >(m_numWorkers);
        std::vector< std::shared_ptr< IWorkHandler< ValueEvent > > > m_handlers = std::vector< std::shared_ptr< IWorkHandler< ValueEvent > > >(m_numWorkers);

        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(),
                                                                                                                  m_bufferSize,
                                                                                                                  std::make_shared< YieldingWaitStrategy >());

        std::shared_ptr< WorkerPool< ValueEvent > > m_workerPool;
    };

} // namespace PerfTests
} // namespace Disruptor
