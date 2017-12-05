#include "stdafx.h"
#include "ThreeToThreeSequencedThroughputTest.h"

#include "Disruptor/YieldingWaitStrategy.h"


namespace Disruptor
{
namespace PerfTests
{

    ThreeToThreeSequencedThroughputTest::ThreeToThreeSequencedThroughputTest()
    {
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            m_buffers[i] = RingBuffer< std::vector< std::int64_t > >::createSingleProducer([this] {return std::vector< std::int64_t >(m_arraySize); },
                                                                                           m_bufferSize,
                                                                                           std::make_shared< YieldingWaitStrategy >());
            m_barriers[i] = m_buffers[i]->newBarrier();
            m_valuePublishers[i] = [this](const std::shared_ptr< Tests::CountdownEvent >& countdownEvent,
                                          const std::shared_ptr< RingBuffer< std::vector< std::int64_t > > >& ringBuffer,
                                          std::int64_t iterations,
                                          std::int32_t arraySize)
            {
                this->valuePublisher(countdownEvent, ringBuffer, iterations, arraySize);
            };
        }

        std::vector< std::shared_ptr< IDataProvider< std::vector< std::int64_t > > > > providers;
        std::copy(m_buffers.begin(), m_buffers.end(), std::back_inserter(providers));

        m_batchEventProcessor = std::make_shared< MultiBufferBatchEventProcessor< std::vector< std::int64_t > > >(providers, m_barriers, m_handler);

        for (auto i = 0; i < m_numPublishers; ++i)
        {
            m_buffers[i]->addGatingSequences({ m_batchEventProcessor->getSequences()[i] });
        }
    }

    std::int64_t ThreeToThreeSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        m_cyclicBarrier->reset();

        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_handler->reset(latch, m_iterations);

        std::vector< std::future< void > > futures(m_numPublishers);
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            auto index = i;
            futures[i] = std::async(std::launch::async, [this, index] { this->m_valuePublishers[index](m_cyclicBarrier, m_buffers[index], m_iterations / m_numPublishers, m_arraySize); });
        }
        auto processorTask = std::async(std::launch::async, [this] { this->m_batchEventProcessor->run(); });

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

        return m_iterations * m_arraySize;
    }

    std::int32_t ThreeToThreeSequencedThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

    void ThreeToThreeSequencedThroughputTest::valuePublisher(const std::shared_ptr< Tests::CountdownEvent >& countdownEvent,
                                                             const std::shared_ptr< RingBuffer< std::vector< std::int64_t > > >& ringBuffer,
                                                             std::int64_t iterations,
                                                             std::int32_t arraySize)
    {
        auto& rb = *ringBuffer;

        countdownEvent->signal();
        countdownEvent->wait();

        for (std::int64_t i = 0; i < iterations; ++i)
        {
            auto sequence = rb.next();
            auto& eventData = rb[sequence];
            for (auto j = 0; j < arraySize; ++j)
            {
                eventData[j] = i + j;
            }
            rb.publish(sequence);
        }
    }

} // namespace PerfTests
} // namespace Disruptor
