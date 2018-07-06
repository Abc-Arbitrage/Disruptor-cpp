#pragma once

#include "Disruptor/Disruptor.h"
#include "Disruptor/ITaskScheduler.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/MutableLong.h"
#include "Disruptor.PerfTests/PerfTestUtil.h"
#include "Disruptor.PerfTests/ValueAdditionEventHandler.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     *
     *     UniCast a series of items between 1 publisher and 1 event processor using the EventTranslator API
     *
     *     +----+    +-----+
     *     | P1 |--->| EP1 |
     *     +----+    +-----+
     *
     *     Disruptor:
     *     ==========
     *     track to prevent wrap
     *     +------------------+
     *     |                  |
     *     |                  v
     *     +----+    +====+    +====+   +-----+
     *     | P1 |--->| RB |/---| SB |   | EP1 |
     *     +----+    +====+    +====+   +-----+
     *     claim      get    ^        |
     *     |        |
     *     +--------+
     *     waitFor
     *
     *     P1  - Publisher 1
     *     RB  - RingBuffer
     *     SB  - SequenceBarrier
     *     EP1 - EventProcessor 1
     *
     */
    class OneToOneTranslatorThroughputTest : public IThroughputTest
    {
        class Translator;

    public:
        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        void waitForEventProcessorSequence(std::int64_t expectedCount);

        const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        const std::int64_t m_expectedResult = PerfTestUtil::accumulatedAddition(m_iterations);
        std::shared_ptr< ValueAdditionEventHandler > m_handler = std::make_shared< ValueAdditionEventHandler >();
        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
        MutableLong m_value = MutableLong(0);
        std::shared_ptr< ITaskScheduler > m_taskScheduler;
        std::shared_ptr< disruptor< ValueEvent > > m_disruptor;
    };


    class OneToOneTranslatorThroughputTest::Translator : public IEventTranslatorVararg< ValueEvent, MutableLong >
    {
    public:
        static const std::shared_ptr< Translator >& instance();

        void translateTo(ValueEvent& event, std::int64_t sequence, const MutableLong& arg0) override;
    };

} // namespace PerfTests
} // namespace Disruptor
