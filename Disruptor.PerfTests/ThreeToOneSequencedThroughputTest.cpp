#include "stdafx.h"
#include "ThreeToOneSequencedThroughputTest.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"


namespace Disruptor
{
namespace PerfTests
{

    ThreeToOneSequencedThroughputTest::ThreeToOneSequencedThroughputTest()
    {
        m_sequenceBarrier = m_ringBuffer->newBarrier();
        m_batchEventProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_ringBuffer, m_sequenceBarrier, m_handler);
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            m_valuePublishers[i] = [this](const std::shared_ptr< Tests::CountdownEvent >& countdownEvent,
                                          const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
                                          std::int64_t iterations)
            {
                this->valuePublisher(countdownEvent, ringBuffer, iterations);
            };
        }
        m_ringBuffer->addGatingSequences({ m_batchEventProcessor->sequence() });
    }

    std::int64_t ThreeToOneSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        m_scheduler->start(5);
        TestTools::ScopeExitFunctor atScopeExit([this] { m_scheduler->stop(); });

        m_cyclicBarrier->reset();
        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_handler->reset(latch, m_batchEventProcessor->sequence()->value() + ((m_iterations / m_numPublishers) * m_numPublishers));

        std::vector< std::future< void > > futures(m_numPublishers);
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            auto index = i;
            futures[i] = m_scheduler->scheduleAndStart(std::packaged_task< void() >([this, index] { m_valuePublishers[index](m_cyclicBarrier, m_ringBuffer, m_iterations); }));
        }
        auto processorTask = m_scheduler->scheduleAndStart(std::packaged_task< void() >([this] { m_batchEventProcessor->run(); }));

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

    std::int32_t ThreeToOneSequencedThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

    void ThreeToOneSequencedThroughputTest::valuePublisher(const std::shared_ptr< Tests::CountdownEvent >& countdownEvent,
                                                           const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
                                                           std::int64_t iterations)
    {
        auto& rb = *ringBuffer;

        countdownEvent->signal();
        countdownEvent->wait();

        for (std::int64_t i = 0; i < iterations; ++i)
        {
            auto sequence = rb.next();
            auto& eventData = rb[sequence];
            eventData.value = i;
            rb.publish(sequence);
        }
    }

} // namespace PerfTests
} // namespace Disruptor
