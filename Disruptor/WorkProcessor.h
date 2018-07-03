#pragma once

#include <atomic>
#include <cstdint>
#include <limits>
#include <memory>

#include "Disruptor/AlertException.h"
#include "Disruptor/IEventProcessor.h"
#include "Disruptor/IEventReleaseAware.h"
#include "Disruptor/IEventReleaser.h"
#include "Disruptor/IExceptionHandler.h"
#include "Disruptor/ILifecycleAware.h"
#include "Disruptor/InvalidOperationException.h"
#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/ITimeoutHandler.h"
#include "Disruptor/IWorkHandler.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/TimeoutException.h"


namespace Disruptor
{
    /**
     * A WorkProcessor<T> wraps a single IWorkHandler<T>, effectively consuming the sequence and ensuring appropriate barriers.
     * Generally, this will be used as part of a WorkerPool<T>
     *
     * \tparam T event implementation storing the details for the work to processed.
     */ 
    template <class T>
    class WorkProcessor : public IEventProcessor
    {
        class EventReleaser;

        struct PrivateKey {};

    public:
        /**
         * Construct a WorkProcessor<T>.
         * 
         * \param ringBuffer ringBuffer to which events are published.
         * \param sequenceBarrier sequenceBarrier on which it is waiting.
         * \param workHandler workHandler is the delegate to which events are dispatched.
         * \param exceptionHandler exceptionHandler to be called back when an error occurs
         * \param workSequence workSequence from which to claim the next event to be worked on. It should always be initialised Disruptor.Sequence.InitialCursorValue
         */ 
        static std::shared_ptr< WorkProcessor< T > > create(const std::shared_ptr< RingBuffer< T > >& ringBuffer,
                                                            const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                                                            const std::shared_ptr< IWorkHandler< T > >& workHandler,
                                                            const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler,
                                                            const std::shared_ptr< ISequence >& workSequence)
        {
            auto processor = std::make_shared< WorkProcessor< T > >(ringBuffer, sequenceBarrier, workHandler, exceptionHandler, workSequence, PrivateKey());
            processor->m_eventReleaser = std::make_shared< EventReleaser >(processor);

            auto eventReleaseAwareHandler = std::dynamic_pointer_cast< IEventReleaseAware >(processor->m_workHandler);
            if (eventReleaseAwareHandler != nullptr)
                eventReleaseAwareHandler->setEventReleaser(processor->m_eventReleaser);

            return processor;
        }

        /**
         * Construct a WorkProcessor<T>.
         * 
         * \param ringBuffer ringBuffer to which events are published.
         * \param sequenceBarrier sequenceBarrier on which it is waiting.
         * \param workHandler workHandler is the delegate to which events are dispatched.
         * \param exceptionHandler exceptionHandler to be called back when an error occurs
         * \param workSequence workSequence from which to claim the next event to be worked on.  It should always be initialised Disruptor.Sequence.InitialCursorValue
         */ 
        WorkProcessor(const std::shared_ptr< RingBuffer< T > >& ringBuffer,
                      const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                      const std::shared_ptr< IWorkHandler< T > >& workHandler,
                      const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler,
                      const std::shared_ptr< ISequence >& workSequence,
                      PrivateKey)
        {
            m_ringBuffer = ringBuffer;
            m_sequenceBarrier = sequenceBarrier;
            m_workHandler = workHandler;
            m_exceptionHandler = exceptionHandler;
            m_workSequence = workSequence;

            m_timeoutHandler = std::dynamic_pointer_cast< ITimeoutHandler >(m_workHandler);
        }

        /**
         * Return a reference to the IEventProcessor.Sequence being used by this IEventProcessor
         */ 
        std::shared_ptr< ISequence > sequence() const override
        {
            return m_sequence;
        }

        /**
         * Signal that this IEventProcessor should stop when it has finished consuming at the next clean break. It will call ISequenceBarrier::alert() to notify the thread to check status.
         */ 
        void halt() override
        {
            m_running = 0;
            m_sequenceBarrier->alert();
        }

        /**
         * \see IEventProcessor::IsRunning()
         */ 
        bool isRunning() const override
        {
            return m_running == 1;
        }

        /**
         * It is ok to have another thread re-run this method after a halt().
         */ 
        void run() override
        {
            if (std::atomic_exchange(&m_running, 1) != 0)
            {
                DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("Thread is already running");
            }
            m_sequenceBarrier->clearAlert();

            notifyStart();

            auto processedSequence = true;
            auto cachedAvailableSequence = std::numeric_limits< std::int64_t >::min();
            auto nextSequence = m_sequence->value();

            T* eventRef = nullptr;

            while (true)
            {
                try
                {
                    if (processedSequence)
                    {
                        processedSequence = false;
                        do
                        {
                            nextSequence = m_workSequence->value() + 1L;
                            m_sequence->setValue(nextSequence - 1L);
                        } while (!m_workSequence->compareAndSet(nextSequence - 1L, nextSequence));
                    }

                    if (cachedAvailableSequence >= nextSequence)
                    {
                        eventRef = &(*m_ringBuffer)[nextSequence];
                        m_workHandler->onEvent(*eventRef);
                        processedSequence = true;
                    }
                    else
                    {
                        cachedAvailableSequence = m_sequenceBarrier->waitFor(nextSequence);
                    }
                }
                catch (TimeoutException&)
                {
                    notifyTimeout(m_sequence->value());
                }
                catch (AlertException&)
                {
                    if (m_running == 0)
                    {
                        break;
                    }
                }
                catch (std::exception& ex)
                {
                    m_exceptionHandler->handleEventException(ex, nextSequence, *eventRef);
                    processedSequence = true;
                }
            }

            notifyShutdown();

            m_running = 0;
        }
        
    private:
        void notifyTimeout(std::int64_t availableSequence)
        {
            try
            {
                if (m_timeoutHandler != nullptr)
                    m_timeoutHandler->onTimeout(availableSequence);
            }
            catch (std::exception& ex)
            {
                m_exceptionHandler->handleOnTimeoutException(ex, availableSequence);
            }
        }

        void notifyStart()
        {
            auto lifecycleAware = std::dynamic_pointer_cast< ILifecycleAware >(m_workHandler);
            if (lifecycleAware != nullptr)
            {
                try
                {
                    lifecycleAware->onStart();
                }
                catch (std::exception& ex)
                {
                    m_exceptionHandler->handleOnStartException(ex);
                }
            }
        }

        void notifyShutdown()
        {
            auto lifecycleAware = std::dynamic_pointer_cast< ILifecycleAware >(m_workHandler);
            if (lifecycleAware != nullptr)
            {
                try
                {
                    lifecycleAware->onShutdown();
                }
                catch (std::exception& ex)
                {
                    m_exceptionHandler->handleOnShutdownException(ex);
                }
            }
        }

        std::atomic< std::int32_t > m_running {0};
        std::shared_ptr< Sequence > m_sequence = std::make_shared< Sequence >();
        std::shared_ptr< RingBuffer< T > > m_ringBuffer;
        std::shared_ptr< ISequenceBarrier > m_sequenceBarrier;
        std::shared_ptr< IWorkHandler< T > > m_workHandler;
        std::shared_ptr< IExceptionHandler< T > > m_exceptionHandler;
        std::shared_ptr< ISequence > m_workSequence;
        std::shared_ptr< IEventReleaser > m_eventReleaser;
        std::shared_ptr< ITimeoutHandler > m_timeoutHandler;
    };

    template <class T>
    class WorkProcessor< T >::EventReleaser : public IEventReleaser
    {
    public:
        explicit EventReleaser(const std::shared_ptr< WorkProcessor< T > >& workProcessor)
            : m_workProcessor(workProcessor)
        {
        }

        void release() override
        {
            auto workProcessor = m_workProcessor.lock();
            if (workProcessor != nullptr)
                workProcessor->sequence()->setValue(std::numeric_limits< std::int64_t >::max());
        }

        private:
        std::weak_ptr< WorkProcessor< T > > m_workProcessor;
    };

} // namespace Disruptor
