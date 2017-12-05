#pragma once

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

    class OneToThreeReleasingWorkerPoolThroughputTest : public IThroughputTest
    {
        class EventCountingAndReleasingWorkHandler;

    public:
        OneToThreeReleasingWorkerPoolThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        void resetCounters();
        std::int64_t sumCounters();

        const std::int32_t m_numWorkers = 3;
        const std::int32_t m_bufferSize = 1024 * 8;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 1L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 10L;
#endif

        std::vector< std::shared_ptr< PaddedLong > > m_counters = std::vector< std::shared_ptr< PaddedLong > >(m_numWorkers);
        std::vector< std::shared_ptr< IWorkHandler< ValueEvent > > > m_handlers = std::vector< std::shared_ptr< IWorkHandler< ValueEvent > > >(m_numWorkers);
        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(),
                                                                                                                  m_bufferSize,
                                                                                                                  std::make_shared< YieldingWaitStrategy >());
        std::shared_ptr< WorkerPool< ValueEvent > > m_workerPool;
    };


    class OneToThreeReleasingWorkerPoolThroughputTest::EventCountingAndReleasingWorkHandler : public IWorkHandler< ValueEvent >, public IEventReleaseAware
    {
    public:
        EventCountingAndReleasingWorkHandler(const std::vector< std::shared_ptr< PaddedLong > >& counters, std::int32_t index);

        void onEvent(ValueEvent& evt) override;

        void setEventReleaser(const std::shared_ptr< IEventReleaser >& eventReleaser) override;

    private:
        std::vector< std::shared_ptr< PaddedLong > > m_counters;
        std::int32_t m_index;
        std::shared_ptr< IEventReleaser > m_eventReleaser;
    };

} // namespace PerfTests
} // namespace Disruptor
