#pragma once

#include "Disruptor/BusySpinWaitStrategy.h"
#include "Disruptor/IWorkHandler.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/WorkProcessor.h"

#include "Disruptor.TestTools/CountdownEvent.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     *
     * Sequence a series of events from multiple publishers going to multiple work processors.
     * 
     * +----+                  +-----+
     * | P1 |---+          +-->| WP1 |
     * +----+   |  +-----+ |   +-----+
     *          +->| RB1 |-+
     * +----+   |  +-----+ |   +-----+
     * | P2 |---+          +-->| WP2 |
     * +----+                  +-----+
     * 
     * P1  - Publisher 1
     * P2  - Publisher 2
     * RB  - RingBuffer
     * WP1 - EventProcessor 1
     * WP2 - EventProcessor 2
     * 
     */
    class TwoToTwoWorkProcessorThroughputTest : public IThroughputTest
    {
        class ValuePublisher;
        class ValueAdditionWorkHandler;

    public:
        TwoToTwoWorkProcessorThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        const std::int32_t m_numPublishers = 2;
        const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 1L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 1L;
#endif

        std::shared_ptr< Tests::CountdownEvent > m_cyclicBarrier = std::make_shared< Tests::CountdownEvent >(m_numPublishers + 1);

        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer = RingBuffer< ValueEvent >::createMultiProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< BusySpinWaitStrategy >());
        std::shared_ptr< Sequence > m_workSequence = std::make_shared< Sequence >();
        std::vector< std::shared_ptr< ValueAdditionWorkHandler > > m_handlers = std::vector< std::shared_ptr< ValueAdditionWorkHandler > >(2);
        std::vector< std::shared_ptr< WorkProcessor< ValueEvent > > > m_workProcessors = std::vector< std::shared_ptr< WorkProcessor< ValueEvent > > >(2);
        std::vector< std::shared_ptr< ValuePublisher > > m_valuePublishers = std::vector< std::shared_ptr< ValuePublisher > >(m_numPublishers);
    };


    class TwoToTwoWorkProcessorThroughputTest::ValuePublisher
    {
    public:
        ValuePublisher(const std::shared_ptr< Tests::CountdownEvent >& cyclicBarrier,
                       const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
                       std::int64_t iterations);

        void run();

    private:
        std::shared_ptr< Tests::CountdownEvent > m_cyclicBarrier;
        const std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
        const std::int64_t m_iterations;
    };


    class TwoToTwoWorkProcessorThroughputTest::ValueAdditionWorkHandler : public IWorkHandler< ValueEvent >
    {
    public:
        void onEvent(ValueEvent& evt) override;

        std::int64_t total() const;

    private:
        std::int64_t m_total = 0;
    };

} // namespace PerfTests
} // namespace Disruptor
