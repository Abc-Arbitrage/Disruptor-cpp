#pragma once

#include <boost/any.hpp>

#include "Disruptor/ISequencer.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/SingleProducerSequencer.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ManualResetEvent.h"
#include "Disruptor.TestTools/Stopwatch.h"

#include "Disruptor.PerfTests/IThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     *  UniCast a series of items between 1 publisher and 1 event processor.
     *  +----+    +-----+
     *  | P1 |--->| EP1 |
     *  +----+    +-----+
     *  
     *  Queue Based:
     *  ============
     *  
     *         put take
     *  +----+    +====+    +-----+
     *  | P1 |---\| Q1 |/---| EP1 |
     *  +----+    +====+    +-----+
     *  
     *  P1  - Publisher 1
     *  Q1  - Queue 1
     *  EP1 - EventProcessor 1
     *  
     *  Disruptor:
     *  ==========
     *               track to prevent wrap
     *               +------------------+
     *               |                  |
     *               |                  v
     *  +----+    +====+    +====+   +-----+
     *  | P1 |--->| RB |/---| SB |   | EP1 |
     *  +----+    +====+    +====+   +-----+
     *       claim get    ^        |
     *                         |        |
     *                         +--------+
     *                           waitFor
     *  P1  - Publisher 1
     *  RB  - RingBuffer
     *  SB  - SequenceBarrier
     *  EP1 - EventProcessor 1
     * 
     */
    class OneToOneRawThroughputTest : public IThroughputTest
    {
        //C# Run: Ops:  140 078 898 - Duration: 1 428 (ms)
        class MyRunnable;

    public:
        OneToOneRawThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;
        std::int32_t requiredProcessorCount() const override;

    private:
        void waitForEventProcessorSequence(std::int64_t expectedCount) const;

        const std::int32_t m_bufferSize = 1024 * 64;

//#ifdef _DEBUG
//        const std::int64_t m_iterations = 100L * 100L * 20L;
//#else
        const std::int64_t m_iterations = 1000L * 1000L * 200L;
//#endif

        std::shared_ptr< ISequencer< boost::any > > m_sequencer = std::make_shared< SingleProducerSequencer< boost::any > >(m_bufferSize, std::make_shared< YieldingWaitStrategy >());
        std::shared_ptr< MyRunnable > m_myRunnable;
        std::shared_ptr< RoundRobinThreadAffinedTaskScheduler > m_taskScheduler;
    };


    class OneToOneRawThroughputTest::MyRunnable
    {
    public:
        explicit MyRunnable(const std::shared_ptr< ISequencer< boost::any > >& sequencer);

        void reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t expectedCount);

        void run() const;

        std::shared_ptr< Sequence > sequence = std::make_shared< Sequence >(-1);

    private:
        std::shared_ptr< Tests::ManualResetEvent > m_latch;
        std::int64_t m_expectedCount = 0;
        std::shared_ptr< ISequenceBarrier > m_barrier;
    };

} // namespace PerfTests
} // namespace Disruptor
