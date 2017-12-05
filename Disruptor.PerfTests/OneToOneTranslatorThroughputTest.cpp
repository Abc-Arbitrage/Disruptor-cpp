#include "stdafx.h"
#include "OneToOneTranslatorThroughputTest.h"

#include "Disruptor/Disruptor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"


namespace Disruptor
{
namespace PerfTests
{

    std::int64_t OneToOneTranslatorThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor stopTaskSchedulerAtScopeExit([this] { m_taskScheduler->stop(); });

        m_disruptor = std::make_shared< ::Disruptor::disruptor< ValueEvent > >(ValueEvent::eventFactory(),
                                                                                  m_bufferSize,
                                                                                  m_taskScheduler,
                                                                                  ProducerType::Single,
                                                                                  std::make_shared< YieldingWaitStrategy >());
        TestTools::ScopeExitFunctor stopDisruptorAtScopeExit([this] { m_disruptor->shutdown(std::chrono::seconds(10)); });

        m_disruptor->handleEventsWith(m_handler);
        m_ringBuffer = m_disruptor->start();

        auto& value = m_value;

        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        auto expectedCount = m_ringBuffer->getMinimumGatingSequence() + m_iterations;

        m_handler->reset(latch, expectedCount);
        stopwatch.start();

        auto& rb = *m_ringBuffer;

        for (std::int64_t l = 0; l < m_iterations; ++l)
        {
            value.value = l;
            rb.publishEvent(Translator::instance(), value);
        }

        latch->waitOne();
        stopwatch.stop();
        waitForEventProcessorSequence(expectedCount);

        PerfTestUtil::failIfNot(m_expectedResult, m_handler->value());

        return m_iterations;
    }

    std::int32_t OneToOneTranslatorThroughputTest::requiredProcessorCount() const
    {
        return 2;
    }

    void OneToOneTranslatorThroughputTest::waitForEventProcessorSequence(std::int64_t expectedCount)
    {
        while (m_ringBuffer->getMinimumGatingSequence() != expectedCount)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    const std::shared_ptr< OneToOneTranslatorThroughputTest::Translator >& OneToOneTranslatorThroughputTest::Translator::instance()
    {
        static std::shared_ptr< Translator > result(std::make_shared< Translator >());
        return result;
    }

    void OneToOneTranslatorThroughputTest::Translator::translateTo(ValueEvent& event, std::int64_t /*sequence*/, const MutableLong& arg0)
    {
        event.value = arg0.value;
    }

} // namespace PerfTests
} // namespace Disruptor
