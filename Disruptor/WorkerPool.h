#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>

#include "Disruptor/BlockingWaitStrategy.h"
#include "Disruptor/InvalidOperationException.h"
#include "Disruptor/IExecutor.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/WorkProcessor.h"


namespace Disruptor
{
    /**
     * WorkerPool contains a pool of WorkProcessor<T> that will consume sequences so jobs can be farmed out across a pool of workers. Each of the WorkProcessor<T> 
     * manage and calls a IWorkHandler<T> to process the events.
     * 
     * \tparam T event to be processed by a pool of workers
     */ 
    template <class T>
    class WorkerPool
    {
    public:
        /**
         * Create a worker pool to enable an array of IWorkHandler<T> to consume published sequences. This option requires a pre-configured RingBuffer<T> which must have
         * Sequencer::setGatingSequences() called before the work pool is started.
         * 
         * \param ringBuffer ringBuffer of events to be consumed.
         * \param sequenceBarrier sequenceBarrier on which the workers will depend.
         * \param exceptionHandler exceptionHandler to callback when an error occurs which is not handled by the<see cref="IWorkHandler{T}"/>s.
         * \param workHandlers workHandlers to distribute the work load across.
         */ 
        WorkerPool(const std::shared_ptr< RingBuffer< T > >& ringBuffer,
                   const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                   const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler,
                   const std::vector< std::shared_ptr< IWorkHandler< T > > >& workHandlers)
        {
            m_ringBuffer = ringBuffer;
            m_workProcessors.resize(workHandlers.size());

            for (auto i = 0u; i < workHandlers.size(); ++i)
            {
                m_workProcessors[i] = WorkProcessor< T >::create(ringBuffer, sequenceBarrier, workHandlers[i], exceptionHandler, m_workSequence);
            }
        }

        /**
         * Construct a work pool with an internal RingBuffer<T> for convenience. This option does not require Sequencer::setGatingSequences() to be called before the work pool is started.
         * 
         * \param eventFactory eventFactory for filling the<see cref="RingBuffer{T}"/>
         * \param exceptionHandler exceptionHandler to callback when an error occurs which is not handled by the<see cref="IWorkHandler{T}"/>s.
         * \param workHandlers workHandlers to distribute the work load across.
         */ 
        WorkerPool(const std::function< T() >& eventFactory,
                   const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler,
                   const std::vector< std::shared_ptr< IWorkHandler< T > > >& workHandlers)

        {
            m_ringBuffer = RingBuffer< T >::createMultiProducer(eventFactory, 1024, std::make_shared< BlockingWaitStrategy >());
            auto barrier = m_ringBuffer->newBarrier();
            m_workProcessors.resize(workHandlers.size());

            for (auto i = 0u; i < workHandlers.size(); ++i)
            {
                m_workProcessors[i] = WorkProcessor< T >::create(m_ringBuffer, barrier, workHandlers[i], exceptionHandler, m_workSequence);
            }

            m_ringBuffer->addGatingSequences(getWorkerSequences());
        }

        /**
         * Get an array of Sequences representing the progress of the workers.
         */ 
        std::vector< std::shared_ptr< ISequence > > getWorkerSequences()
        {
            std::vector< std::shared_ptr< ISequence > > sequences(m_workProcessors.size() + 1);
            for (auto i = 0u; i < m_workProcessors.size(); ++i)
            {
                sequences[i] = m_workProcessors[i]->sequence();
            }
            sequences[sequences.size() - 1] = m_workSequence;

            return sequences;
        }

        /**
         * Start the worker pool processing events in sequence.
         * 
         * \returns the RingBuffer<T> used for the work queue.
         */ 
        std::shared_ptr< RingBuffer< T > > start(const std::shared_ptr< IExecutor >& executor)
        {
            if (std::atomic_exchange(&m_running, 1) != 0)
            {
                DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("WorkerPool has already been started and cannot be restarted until halted");
            }

            auto cursor = m_ringBuffer->cursor();
            m_workSequence->setValue(cursor);

            for (auto&& workProcessor : m_workProcessors)
            {
                workProcessor->sequence()->setValue(cursor);
                executor->execute([workProcessor] { workProcessor->run(); });
            }

            return m_ringBuffer;
        }

        /**
         * Wait for the RingBuffer<T> to drain of published events then halt the workers.
         */ 
        void drainAndHalt()
        {
            auto workerSequences = getWorkerSequences();
            while (m_ringBuffer->cursor() > Util::getMinimumSequence(workerSequences))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(0));
            }

            for (auto&& workProcessor : m_workProcessors)
            {
                workProcessor->halt();
            }

            m_running = 0;
        }

        /**
         * Halt all workers immediately at then end of their current cycle.
         */ 
        void halt()
        {
            for (auto&& workProcessor : m_workProcessors)
            {
                workProcessor->halt();
            }

            m_running = 0;
        }

        bool isRunning() const
        {
            return m_running == 1;
        }

    private:
        std::atomic< std::int32_t > m_running {0};
        std::shared_ptr< Sequence > m_workSequence = std::make_shared< Sequence >();
        std::shared_ptr< RingBuffer< T > > m_ringBuffer;
        // WorkProcessors are created to wrap each of the provided WorkHandlers
        std::vector< std::shared_ptr< WorkProcessor< T > > > m_workProcessors;
    };

} // namespace Disruptor
