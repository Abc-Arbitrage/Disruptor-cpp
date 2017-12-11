#include "stdafx.h"
#include "TwoToTwoWorkProcessorThroughputTest.h"

#include "Disruptor/IgnoreExceptionHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    TwoToTwoWorkProcessorThroughputTest::TwoToTwoWorkProcessorThroughputTest()
    {
        auto sequenceBarrier = m_ringBuffer->newBarrier();
        m_handlers[0] = std::make_shared< ValueAdditionWorkHandler >();
        m_handlers[1] = std::make_shared< ValueAdditionWorkHandler >();

        m_workProcessors[0] = WorkProcessor< ValueEvent >::create(m_ringBuffer, sequenceBarrier, m_handlers[0], std::make_shared< IgnoreExceptionHandler< ValueEvent > >(), m_workSequence);
        m_workProcessors[1] = WorkProcessor< ValueEvent >::create(m_ringBuffer, sequenceBarrier, m_handlers[1], std::make_shared< IgnoreExceptionHandler< ValueEvent > >(), m_workSequence);

        for (auto i = 0; i < m_numPublishers; ++i)
        {
            m_valuePublishers[i] = std::make_shared< ValuePublisher >(m_cyclicBarrier, m_ringBuffer, m_iterations);
        }

        m_ringBuffer->addGatingSequences({ m_workProcessors[0]->sequence(), m_workProcessors[1]->sequence() });
    }

    std::int64_t TwoToTwoWorkProcessorThroughputTest::run(Stopwatch& stopwatch)
    {
        m_cyclicBarrier->reset();

        auto expected = m_ringBuffer->cursor() + (m_numPublishers * m_iterations);
        auto futures = std::vector< std::future< void > >(m_numPublishers);
        for (auto i = 0; i < m_numPublishers; ++i)
        {
            auto index = i;
            futures[i] = std::async(std::launch::async, [this, index] { m_valuePublishers[index]->run(); });
        }

        for (auto&& processor : m_workProcessors)
        {
            std::thread([processor] { processor->run(); }).detach();
        }

        stopwatch.start();
        m_cyclicBarrier->signal();
        m_cyclicBarrier->wait();

        for (auto i = 0; i < m_numPublishers; ++i)
        {
            futures[i].wait();
        }

        while (m_workSequence->value() < expected)
        {
            std::this_thread::yield();
        }

        stopwatch.stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        for (auto&& processor : m_workProcessors)
        {
            processor->halt();
        }

        return m_iterations;
    }

    std::int32_t TwoToTwoWorkProcessorThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }


    TwoToTwoWorkProcessorThroughputTest::ValuePublisher::ValuePublisher(const std::shared_ptr< Tests::CountdownEvent >& cyclicBarrier,  
                                                                        const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
                                                                        std::int64_t iterations)
        : m_cyclicBarrier(cyclicBarrier)
        , m_ringBuffer(ringBuffer)
        , m_iterations(iterations)
    {
    }

    void TwoToTwoWorkProcessorThroughputTest::ValuePublisher::run()
    {
        m_cyclicBarrier->signal();
        m_cyclicBarrier->wait();

        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto sequence = m_ringBuffer->next();
            auto& event = (*m_ringBuffer)[sequence];
            event.value = i;
            m_ringBuffer->publish(sequence);
        }
    }

    void TwoToTwoWorkProcessorThroughputTest::ValueAdditionWorkHandler::onEvent(ValueEvent& evt)
    {
        auto value = evt.value;
        m_total += value;
    }

    std::int64_t TwoToTwoWorkProcessorThroughputTest::ValueAdditionWorkHandler::total() const
    {
        return m_total;
    }

} // namespace PerfTests
} // namespace Disruptor
