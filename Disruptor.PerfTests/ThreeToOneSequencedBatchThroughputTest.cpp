#include "stdafx.h"
#include "ThreeToOneSequencedBatchThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    ThreeToOneSequencedBatchThroughputTest::ThreeToOneSequencedBatchThroughputTest()
    {
        auto sequenceBarrier = m_ringBuffer->newBarrier();
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            m_valuePublishers[i] = std::make_shared< ValueBatchPublisher >(m_cyclicBarrier, m_ringBuffer, m_iterations / m_numPublishers, 10);
        }

        m_batchEventProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_ringBuffer, sequenceBarrier, m_handler);
        m_ringBuffer->addGatingSequences({ m_batchEventProcessor->sequence() });
    }

    std::int64_t ThreeToOneSequencedBatchThroughputTest::run(Stopwatch& stopwatch)
    {
        m_cyclicBarrier->reset();

        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_handler->reset(latch, m_batchEventProcessor->sequence()->value() + ((m_iterations / m_numPublishers) * m_numPublishers));

        std::vector< std::future< void > > futures(m_numPublishers);
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            auto index = i;
            futures[i] = std::async(std::launch::async, [this, index] { m_valuePublishers[index]->run(); });
        }
        auto processorTask = std::async(std::launch::async, [this] { m_batchEventProcessor->run(); });

        stopwatch.start();
        m_cyclicBarrier->signal();
        m_cyclicBarrier->wait();

        for (auto i = 0; i < m_numPublishers; ++i)
        {
            futures[i].wait();
        }

        latch->waitOne();

        stopwatch.stop();
        m_batchEventProcessor->halt();
        processorTask.wait_for(std::chrono::milliseconds(2000));

        return m_iterations;
    }

    std::int32_t ThreeToOneSequencedBatchThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

    ThreeToOneSequencedBatchThroughputTest::ValueBatchPublisher::ValueBatchPublisher(const std::shared_ptr< Tests::CountdownEvent >& cyclicBarrier,
                                                                                     const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
                                                                                     std::int64_t iterations,
                                                                                     std::int32_t batchSize)
        : m_cyclicBarrier(cyclicBarrier)
        , m_ringBuffer(ringBuffer)
        , m_iterations(iterations)
        , m_batchSize(batchSize)
    {
    }

    void ThreeToOneSequencedBatchThroughputTest::ValueBatchPublisher::run()
    {
        auto& rb = *m_ringBuffer;

        m_cyclicBarrier->signal();
        m_cyclicBarrier->wait();

        for (std::int64_t i = 0; i < m_iterations; i += m_batchSize)
        {
            auto hi = m_ringBuffer->next(m_batchSize);
            auto lo = hi - (m_batchSize - 1);
            for (auto l = lo; l <= hi; ++l)
            {
                auto& event = rb[l];
                event.value = l;
            }
            m_ringBuffer->publish(lo, hi);
        }
    }

} // namespace PerfTests
} // namespace Disruptor
