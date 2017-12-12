#include "stdafx.h"
#include "OneToThreeSequencedThroughputTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"
#include "Disruptor/BatchEventProcessor.h"
#include "PerfTestUtil.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToThreeSequencedThroughputTest::OneToThreeSequencedThroughputTest()
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_executor = std::make_shared< BasicExecutor >(m_taskScheduler);

        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            m_results[0] = OperationExtensions::Op(Operation::Addition, m_results[0], i);
            m_results[1] = OperationExtensions::Op(Operation::Subtraction, m_results[1], i);
            m_results[2] = OperationExtensions::Op(Operation::And, m_results[2], i);
        }

        m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());
        auto sequenceBarrier = m_ringBuffer->newBarrier();

        m_handlers[0] = std::make_shared< ValueMutationEventHandler >(Operation::Addition);
        m_handlers[1] = std::make_shared< ValueMutationEventHandler >(Operation::Subtraction);
        m_handlers[2] = std::make_shared< ValueMutationEventHandler >(Operation::And);

        for (auto i = 0; i < m_numEventProcessors; ++i)
        {
            m_batchEventProcessors[i] = std::make_shared< BatchEventProcessor< ValueEvent > >(m_ringBuffer, sequenceBarrier, m_handlers[i]);
        }

        std::vector< std::shared_ptr< ISequence > > sequences;
        for (auto&& processor : m_batchEventProcessors)
        {
            sequences.push_back(processor->sequence());
        }
        m_ringBuffer->addGatingSequences(sequences);
    }

    std::int64_t OneToThreeSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto latch = std::make_shared< boost::barrier >(m_numEventProcessors + 1);

        std::vector< std::future< void > > processorTasks;
        for (auto i = 0; i < m_numEventProcessors; ++i)
        {
            m_handlers[i]->reset(latch, m_batchEventProcessors[i]->sequence()->value() + m_iterations);
            processorTasks.push_back(m_executor->execute([this, i] { m_batchEventProcessors[i]->run(); }));
        }

        auto& rb = *m_ringBuffer;

        stopwatch.start();

        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto sequence = rb.next();
            rb[sequence].value = i;
            rb.publish(sequence);
        }

        latch->wait();
        stopwatch.stop();

        for (auto i = 0; i < m_numEventProcessors; ++i)
        {
            m_batchEventProcessors[i]->halt();
            PerfTestUtil::failIfNot(m_results[i], m_handlers[i]->value(),
                                    "Result " + std::to_string(m_results[i]) + " != " + std::to_string(m_handlers[i]->value()));
        }

        for (auto&& task : processorTasks)
            task.wait();

        return m_numEventProcessors * m_iterations;
    }

    std::int32_t OneToThreeSequencedThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

} // namespace PerfTests
} // namespace Disruptor
