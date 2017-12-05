#pragma once

#include "Disruptor/AlertException.h"
#include "Disruptor/ArgumentNullException.h"
#include "Disruptor/IDataProvider.h"
#include "Disruptor/IEventHandler.h"
#include "Disruptor/IEventProcessor.h"
#include "Disruptor/IExceptionHandler.h"
#include "Disruptor/ILifecycleAware.h"
#include "Disruptor/InvalidOperationException.h"
#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/ISequenceReportingEventHandler.h"
#include "Disruptor/ITimeoutHandler.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/TimeoutException.h"


namespace Disruptor
{

    /// <summary>
    /// Convenience class for handling the batching semantics of consuming events from a <see cref="RingBuffer{T}"/>
    /// and delegating the available events to an <see cref="IEventHandler{T}"/>.
    /// 
    /// If the <see cref="BatchEventProcessor{T}"/> also implements <see cref="ILifecycleAware"/> it will be notified just after the thread
    /// is started and just before the thread is shutdown.
    /// </summary>
    /// <typeparam name="T">Event implementation storing the data for sharing during exchange or parallel coordination of an event.</typeparam>
    template <class T>
    class BatchEventProcessor : public IEventProcessor
    {
    public:
        /// <summary>
        /// Construct a <see cref="BatchEventProcessor{T}"/> that will automatically track the progress by updating its sequence when
        /// the <see cref="IEventHandler{T}.OnEvent"/> method returns.
        /// </summary>
        /// <param name="dataProvider">dataProvider to which events are published</param>
        /// <param name="sequenceBarrier">SequenceBarrier on which it is waiting.</param>
        /// <param name="eventHandler">eventHandler is the delegate to which events are dispatched.</param>
        BatchEventProcessor(const std::shared_ptr< IDataProvider< T > >& dataProvider,
                            const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                            const std::shared_ptr< IEventHandler< T > >& eventHandler)
            : m_running(false)
            , m_dataProvider(dataProvider)
            , m_dataProviderRef(*m_dataProvider)
            , m_sequenceBarrier(sequenceBarrier)
            , m_sequenceBarrierRef(*m_sequenceBarrier)
            , m_eventHandler(eventHandler)
            , m_eventHandlerRef(*m_eventHandler)
            , m_sequence(std::make_shared< Sequence >())
            , m_sequenceRef(*m_sequence)
        {
            auto sequenceReportingHandler = std::dynamic_pointer_cast< ISequenceReportingEventHandler< T > >(eventHandler);
            if (sequenceReportingHandler != nullptr)
                sequenceReportingHandler->setSequenceCallback(m_sequence);

            m_timeoutHandler = std::dynamic_pointer_cast< ITimeoutHandler >(eventHandler);
        }

        /// <summary>
        /// <see cref="IEventProcessor.Sequence"/>
        /// </summary>
        std::shared_ptr< ISequence > sequence() const override
        {
            return m_sequence;
        };

        /// <summary>
        /// Signal that this <see cref="IEventProcessor"/> should stop when it has finished consuming at the next clean break.
        /// It will call <see cref="ISequenceBarrier.Alert"/> to notify the thread to check status.
        /// </summary>
        void halt() override
        {
            m_running = false;
            m_sequenceBarrier->alert();
        }

        /// <summary>
        /// <see cref="IEventProcessor.IsRunning"/>
        /// </summary>
        bool isRunning() const override
        {
            return m_running;
        }

        /// <summary>
        /// Set a new <see cref="IExceptionHandler{T}"/> for handling exceptions propagated out of the <see cref="BatchEventProcessor{T}"/>
        /// </summary>
        /// <param name="exceptionHandler">exceptionHandler to replace the existing exceptionHandler.</param>
        void setExceptionHandler(const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler)
        {
            if (exceptionHandler == nullptr)
                DISRUPTOR_THROW_ARGUMENT_NULL_EXCEPTION(exceptionHandler);

            m_exceptionHandler = exceptionHandler;
        }

        /// <summary>
        /// It is ok to have another thread rerun this method after a halt().
        /// </summary>
        void run() override
        {
            if (m_running.exchange(true) != false)
            {
                DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("Thread is already running");
            }
            
            m_sequenceBarrierRef.clearAlert();

            notifyStart();

            auto nextSequence = m_sequenceRef.value() + 1;

            T* evt = nullptr;

            while (true)
            {
                try
                {
                    auto availableSequence = m_sequenceBarrierRef.waitFor(nextSequence);

                    while (nextSequence <= availableSequence)
                    {
                        evt = &m_dataProviderRef[nextSequence];
                        m_eventHandlerRef.onEvent(*evt, nextSequence, nextSequence == availableSequence);
                        nextSequence++;
                    }

                    m_sequenceRef.setValue(availableSequence);
                }
                catch (const TimeoutException&)
                {
                    notifyTimeout(m_sequenceRef.value());
                }
                catch (const AlertException&)
                {
                    if (m_running == false)
                    {
                        break;
                    }
                }
                catch (const std::exception& ex)
                {
                    m_exceptionHandler->handleEventException(ex, nextSequence, *evt);
                    m_sequenceRef.setValue(nextSequence);
                    nextSequence++;
                }
            }

            notifyShutdown();
            m_running = false;
        }

    private:
        void notifyTimeout(std::int64_t availableSequence) const
        {
            try
            {
                if (m_timeoutHandler)
                    m_timeoutHandler->onTimeout(availableSequence);
            }
            catch (std::exception& ex)
            {
                if (m_exceptionHandler)
                    m_exceptionHandler->handleOnTimeoutException(ex, availableSequence);
            }
        }

        void notifyStart()
        {
            auto sequenceReportingHandler = std::dynamic_pointer_cast< ILifecycleAware >(m_eventHandler);
            if (sequenceReportingHandler != nullptr)
            {
                try
                {
                    sequenceReportingHandler->onStart();
                }
                catch (std::exception& ex)
                {
                    if (m_exceptionHandler)
                        m_exceptionHandler->handleOnStartException(ex);
                }
            }
        }

        void notifyShutdown()
        {
            auto sequenceReportingHandler = std::dynamic_pointer_cast< ILifecycleAware >(m_eventHandler);
            if (sequenceReportingHandler != nullptr)
            {
                try
                {
                    sequenceReportingHandler->onShutdown();
                }
                catch (std::exception& ex)
                {
                    if (m_exceptionHandler)
                        m_exceptionHandler->handleOnShutdownException(ex);
                }
            }
        }

    private:
        std::atomic<bool> m_running;
        std::shared_ptr< IDataProvider< T > > m_dataProvider;
        IDataProvider< T >& m_dataProviderRef;
        std::shared_ptr< ISequenceBarrier > m_sequenceBarrier;
        ISequenceBarrier& m_sequenceBarrierRef;
        std::shared_ptr< IEventHandler< T > > m_eventHandler;
        IEventHandler< T >& m_eventHandlerRef;
        std::shared_ptr< Sequence > m_sequence;
        Sequence& m_sequenceRef;
        std::shared_ptr< ITimeoutHandler > m_timeoutHandler;
        std::shared_ptr< IExceptionHandler< T > > m_exceptionHandler;
    };

} // namespace Disruptor
