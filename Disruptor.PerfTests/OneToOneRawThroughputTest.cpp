#include "stdafx.h"
#include "OneToOneRawThroughputTest.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToOneRawThroughputTest::OneToOneRawThroughputTest()
    {
        m_myRunnable = std::make_shared< MyRunnable >(m_sequencer);
        m_sequencer->addGatingSequences({ m_myRunnable->sequence });
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
    }

    std::int64_t OneToOneRawThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        const auto expectedCount = m_myRunnable->sequence->value() + m_iterations;
        m_myRunnable->reset(latch, expectedCount);

        auto consumerTask = m_taskScheduler->scheduleAndStart(std::packaged_task< void() >([this] { m_myRunnable->run(); }));
        stopwatch.start();

        auto sequencer = m_sequencer;

        auto producerTask = m_taskScheduler->scheduleAndStart(std::packaged_task< void() >([this, sequencer, latch]
        {
            auto& s = *sequencer;
            for (std::int64_t i = 0; i < m_iterations; ++i)
            {
                auto next = s.next();
                s.publish(next);
            }

            latch->waitOne();
        }));

        producerTask.wait();
        stopwatch.stop();
        waitForEventProcessorSequence(expectedCount);

        consumerTask.wait();

        return m_iterations;
    }

    std::int32_t OneToOneRawThroughputTest::requiredProcessorCount() const
    {
        return 2;
    }

    void OneToOneRawThroughputTest::waitForEventProcessorSequence(std::int64_t expectedCount) const
    {
        while (m_myRunnable->sequence->value() != expectedCount)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    OneToOneRawThroughputTest::MyRunnable::MyRunnable(const std::shared_ptr< ISequencer< boost::any > >& sequencer)
    {
        m_barrier = sequencer->newBarrier({});
    }

    void OneToOneRawThroughputTest::MyRunnable::reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t expectedCount)
    {
        m_latch = latch;
        m_expectedCount = expectedCount;
    }

    void OneToOneRawThroughputTest::MyRunnable::run() const
    {
        auto expected = m_expectedCount;

        auto& b = *m_barrier;
        auto& s = *sequence;

        try
        {
            std::int64_t processed;

            do
            {
                processed = b.waitFor(s.value() + 1);
                s.setValue(processed);
            }
            while (processed < expected);

            m_latch->set();
            s.setValue(processed);
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

} // namespace PerfTests
} // namespace Disruptor
