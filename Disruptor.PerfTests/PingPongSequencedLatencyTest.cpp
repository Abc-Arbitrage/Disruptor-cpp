#include "stdafx.h"
#include "PingPongSequencedLatencyTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"
#include "LatencyTestSession.h"


namespace Disruptor
{
namespace PerfTests
{

    PingPongSequencedLatencyTest::PingPongSequencedLatencyTest()
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_executor = std::make_shared< BasicExecutor >(m_taskScheduler);

        m_pingBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< BlockingWaitStrategy >());
        m_pongBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< BlockingWaitStrategy >());

        m_pingBarrier = m_pingBuffer->newBarrier();
        m_pongBarrier = m_pongBuffer->newBarrier();

        m_pinger = std::make_shared< Pinger >(m_pingBuffer, m_iterations, m_pauseDurationInNanos);
        m_ponger = std::make_shared< Ponger >(m_pongBuffer);

        m_pingProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_pongBuffer, m_pongBarrier, m_pinger);
        m_pongProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_pingBuffer, m_pingBarrier, m_ponger);

        m_pingBuffer->addGatingSequences({ m_pongProcessor->sequence() });
        m_pongBuffer->addGatingSequences({ m_pingProcessor->sequence() });
    }

    std::int32_t PingPongSequencedLatencyTest::requiredProcessorCount() const
    {
        return 2;
    }

    void PingPongSequencedLatencyTest::run(Stopwatch& stopwatch, const std::shared_ptr< Tests::LatencyRecorder >& latencyRecorder)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto globalSignal = std::make_shared< Tests::CountdownEvent >(3);
        auto signal = std::make_shared< Tests::ManualResetEvent >(false);
        m_pinger->reset(globalSignal, signal, latencyRecorder);
        m_ponger->reset(globalSignal);

        auto processorTask1 = m_executor->execute([this] { m_pongProcessor->run(); });
        auto processorTask2 = m_executor->execute([this] { m_pingProcessor->run(); });

        globalSignal->signal();
        globalSignal->wait();
        stopwatch.start();
        // running here
        signal->waitOne();
        stopwatch.stop();

        m_pingProcessor->halt();
        m_pongProcessor->halt();

        processorTask1.wait();
        processorTask2.wait();
    }


    PingPongSequencedLatencyTest::Pinger::Pinger(const std::shared_ptr< RingBuffer< ValueEvent > >& buffer, std::int64_t maxEvents, std::int32_t pauseDurationInNanos)
        : m_buffer(buffer)
        , m_maxEvents(maxEvents)
        , m_pauseDurationInNanos(pauseDurationInNanos)
    {
        m_pauseDurationInStopwatchTicks = (std::int64_t)LatencyTestSession::convertNanoToStopwatchTicks(pauseDurationInNanos);
    }

    void PingPongSequencedLatencyTest::Pinger::onEvent(ValueEvent& data, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        auto t1 = Stopwatch::getTimestamp();

        m_latencyRecorder->record(LatencyTestSession::convertStopwatchTicksToNano((double)(t1 - m_t0)));

        if (data.value < m_maxEvents)
        {
            while (m_pauseDurationInNanos > (Stopwatch::getTimestamp() - t1))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(0));
            }

            send();
        }
        else
        {
            m_signal->set();
        }
    }

    void PingPongSequencedLatencyTest::Pinger::onStart()
    {
        m_globalSignal->signal();
        m_globalSignal->wait();

        send();
    }

    void PingPongSequencedLatencyTest::Pinger::onShutdown()
    {
    }

    void PingPongSequencedLatencyTest::Pinger::reset(const std::shared_ptr< Tests::CountdownEvent >& globalSignal,
                                                     const std::shared_ptr< Tests::ManualResetEvent >& signal,
                                                     const std::shared_ptr< Tests::LatencyRecorder >& latencyRecorder)
    {
        m_latencyRecorder = latencyRecorder;
        m_globalSignal = globalSignal;
        m_signal = signal;

        m_counter = 0;
    }

    void PingPongSequencedLatencyTest::Pinger::send()
    {
        m_t0 = Stopwatch::getTimestamp();
        auto next = m_buffer->next();
        (*m_buffer)[next].value = m_counter;
        m_buffer->publish(next);

        ++m_counter;
    }


    PingPongSequencedLatencyTest::Ponger::Ponger(const std::shared_ptr< RingBuffer< ValueEvent > >& buffer)
        : m_buffer(buffer)
    {
    }

    void PingPongSequencedLatencyTest::Ponger::onEvent(ValueEvent& data, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        auto next = m_buffer->next();
        (*m_buffer)[next].value = data.value;
        m_buffer->publish(next);
    }

    void PingPongSequencedLatencyTest::Ponger::onStart()
    {
        m_globalSignal->signal();
        m_globalSignal->wait();
    }

    void PingPongSequencedLatencyTest::Ponger::onShutdown()
    {
    }

    void PingPongSequencedLatencyTest::Ponger::reset(const std::shared_ptr< Tests::CountdownEvent >& globalSignal)
    {
        m_globalSignal = globalSignal;
    }

} // namespace PerfTests
} // namespace Disruptor
