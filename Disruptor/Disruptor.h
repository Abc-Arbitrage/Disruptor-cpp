#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/ClockConfig.h"
#include "Disruptor/ConsumerRepository.h"
#include "Disruptor/EventHandlerGroup.h"
#include "Disruptor/ExceptionHandlerSetting.h"
#include "Disruptor/ExceptionHandlerWrapper.h"
#include "Disruptor/IEventHandler.h"
#include "Disruptor/IEventProcessorFactory.h"
#include "Disruptor/InvalidOperationException.h"
#include "Disruptor/ITaskScheduler.h"
#include "Disruptor/IWorkHandler.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/TimeoutException.h"
#include "Disruptor/Util.h"
#include "Disruptor/WorkerPool.h"


namespace Disruptor
{

    /// <summary>
    /// A DSL-style API for setting up the disruptor pattern around a ring buffer
    /// (aka the Builder pattern).
    /// 
    /// A simple example of setting up the disruptor with two event handlers that
    /// must process events in order:
    /// <code>Disruptor{MyEvent} disruptor = new Disruptor{MyEvent}(MyEvent.FACTORY, 32, Executors.NewCachedThreadPool());
    /// EventHandler{MyEvent} handler1 = new EventHandler{MyEvent}() { ... };
    /// EventHandler{MyEvent} handler2 = new EventHandler{MyEvent}() { ... };
    /// disruptor.HandleEventsWith(handler1);
    /// disruptor.After(handler1).HandleEventsWith(handler2);
    /// RingBuffer ringBuffer = disruptor.Start();</code>
    /// </summary>
    /// <typeparam name="T">the type of event used.</typeparam>
    template <class T>
    class disruptor : public std::enable_shared_from_this< disruptor< T > >
    {
        typedef EventHandlerGroup< T, ::Disruptor::disruptor > EventHandlerGroupType;

    public:
        /// <summary>
        /// Create a new Disruptor. Will default to <see cref="BlockingWaitStrategy"/> and
        /// <see cref="ProducerType.Multi"/>
        /// </summary>
        /// <param name="eventFactory">the factory to create events in the ring buffer</param>
        /// <param name="ringBufferSize">the size of the ring buffer</param>
        /// <param name="taskScheduler">a <see cref="TaskScheduler"/> to create threads to for processors</param>
        disruptor(const std::function< T() >& eventFactory, std::int32_t ringBufferSize, const std::shared_ptr< ITaskScheduler >& taskScheduler)
            : disruptor(RingBuffer< T >::createMultiProducer(eventFactory, ringBufferSize), std::make_shared< BasicExecutor >(taskScheduler))
        {
        }

        /// <summary>
        /// Create a new Disruptor.
        /// </summary>
        /// <param name="eventFactory">the factory to create events in the ring buffer</param>
        /// <param name="ringBufferSize">the size of the ring buffer, must be power of 2</param>
        /// <param name="taskScheduler">a <see cref="TaskScheduler"/> to create threads to for processors</param>
        /// <param name="producerType">the claim strategy to use for the ring buffer</param>
        /// <param name="waitStrategy">the wait strategy to use for the ring buffer</param>
        disruptor(const std::function< T() >& eventFactory,
                  std::int32_t ringBufferSize,
                  const std::shared_ptr< ITaskScheduler >& taskScheduler,
                  ProducerType producerType,
                  const std::shared_ptr< IWaitStrategy >& waitStrategy)
            : disruptor(RingBuffer< T >::create(producerType, eventFactory, ringBufferSize, waitStrategy), std::make_shared< BasicExecutor >(taskScheduler))
        {
        }

        /// <summary>
        /// Allows the executor to be specified
        /// </summary>
        /// <param name="eventFactory"></param>
        /// <param name="ringBufferSize"></param>
        /// <param name="executor"></param>
        disruptor(const std::function< T() >& eventFactory, std::int32_t ringBufferSize, const std::shared_ptr< IExecutor >& executor)
            : disruptor(RingBuffer< T >::createMultiProducer(eventFactory, ringBufferSize), executor)
        {
        }

        /// <summary>
        /// Set up event handlers to handle events from the ring buffer. These handlers will process events
        /// as soon as they become available, in parallel.
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// </summary>
        /// <param name="handlers">the event handlers that will process events</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::vector< std::shared_ptr< IEventHandler< T > > >& handlers)
        {
            return createEventProcessors({ }, handlers);
        }

        /// <summary>
        /// Set up event handlers to handle events from the ring buffer. These handlers will process events
        /// as soon as they become available, in parallel.
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// </summary>
        /// <param name="handler">the event handler that will process events</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::shared_ptr< IEventHandler< T > >& handler)
        {
            return handleEventsWith(std::vector< std::shared_ptr< IEventHandler< T > > > { handler });
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Start"/> is called.
        /// 
        /// This method can be used as the start of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// 
        /// Since this is the start of the chain, the processor factories will always be passed an empty <code>Sequence</code>
        /// array, so the factory isn't necessary in this case. This method is provided for consistency with
        /// <see cref="EventHandlerGroup{T}.HandleEventsWith(IEventProcessorFactory{T}[])"/> and <see cref="EventHandlerGroup{T}.Then(IEventProcessorFactory{T}[])"/>
        /// which do have barrier sequences to provide.
        /// </summary>
        /// <param name="eventProcessorFactories">the event processor factories to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::vector< std::shared_ptr< IEventProcessorFactory< T > > >& eventProcessorFactories)
        {
            return createEventProcessors({ }, eventProcessorFactories);
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Start"/> is called.
        /// 
        /// This method can be used as the start of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// 
        /// Since this is the start of the chain, the processor factories will always be passed an empty <code>Sequence</code>
        /// array, so the factory isn't necessary in this case. This method is provided for consistency with
        /// <see cref="EventHandlerGroup{T}.HandleEventsWith(IEventProcessorFactory{T}[])"/> and <see cref="EventHandlerGroup{T}.Then(IEventProcessorFactory{T}[])"/>
        /// which do have barrier sequences to provide.
        /// </summary>
        /// <param name="eventProcessorFactory">the event processor factory to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::shared_ptr< IEventProcessorFactory< T > >& eventProcessorFactory)
        {
            return handleEventsWith(std::vector< std::shared_ptr< IEventProcessorFactory< T > > > { eventProcessorFactory });
        }
        
        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start this processors when <see cref="Start"/> is called.
        /// 
        /// This method can be used as the start of a chain. For example if the processor <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// </summary>
        /// <param name="processors">the event processors that will process events</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::vector< std::shared_ptr< IEventProcessor > >& processors)
        {
            for (auto&& processor : processors)
            {
                m_consumerRepository->add(processor);
            }
            return std::make_shared< EventHandlerGroupType >(this->shared_from_this(), m_consumerRepository, Util::getSequencesFor(processors));
        }
        
        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start this processors when <see cref="Start"/> is called.
        /// 
        /// This method can be used as the start of a chain. For example if the processor <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).Then(B);</code>
        /// </summary>
        /// <param name="processor">the event processor that will process events</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::shared_ptr< IEventProcessor >& processor)
        {
            return handleEventsWith(std::vector< std::shared_ptr< IEventProcessor > > { processor });
        }
        
        /// <summary>
        /// Set up a <see cref="WorkerPool{T}"/> to distribute an event to one of a pool of work handler threads.
        /// Each event will only be processed by one of the work handlers.
        /// The Disruptor will automatically start this processors when <see cref="Start"/> is called.
        /// </summary>
        /// <param name="workHandlers">the work handlers that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWithWorkerPool(const std::vector< std::shared_ptr< IWorkHandler< T > > >& workHandlers)
        {
            return createWorkerPool({}, workHandlers);
        }
        
        /// <summary>
        /// Set up a <see cref="WorkerPool{T}"/> to distribute an event to one of a pool of work handler threads.
        /// Each event will only be processed by one of the work handlers.
        /// The Disruptor will automatically start this processors when <see cref="Start"/> is called.
        /// </summary>
        /// <param name="workHandler">the work handler that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWithWorkerPool(const std::shared_ptr< IWorkHandler< T > >& workHandler)
        {
            return handleEventsWithWorkerPool(std::vector< std::shared_ptr< IWorkHandler< T > > > { workHandler });
        }

        /// <summary>
        /// Specify an exception handler to be used for any future event handlers.
        /// Note that only event handlers set up after calling this method will use the exception handler.
        /// </summary>
        /// <param name="exceptionHandler">the exception handler to use for any future <see cref="IEventProcessor"/>.</param>
        //[Obsolete("This method only applies to future event handlers. Use setDefaultExceptionHandler instead which applies to existing and new event handlers.")]
        void handleExceptionsWith(const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler)
        {
            m_exceptionHandler = exceptionHandler;
        }

        /// <summary>
        /// Specify an exception handler to be used for event handlers and worker pools created by this Disruptor.
        /// The exception handler will be used by existing and future event handlers and worker pools created by this Disruptor instance.
        /// </summary>
        /// <param name="exceptionHandler">the exception handler to use</param>
        void setDefaultExceptionHandler(const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler)
        {
            checkNotStarted();
            std::dynamic_pointer_cast< ExceptionHandlerWrapper< T > >(m_exceptionHandler)->switchTo(exceptionHandler);
        }
        
        /// <summary>
        /// Override the default exception handler for a specific handler.
        /// <code>disruptorWizard.HandleExceptionsIn(eventHandler).With(exceptionHandler);</code>
        /// </summary>
        /// <param name="eventHandler">eventHandler the event handler to set a different exception handler for</param>
        /// <returns>an <see cref="ExceptionHandlerSetting{T}"/> dsl object - intended to be used by chaining the with method call</returns>
        std::shared_ptr< ExceptionHandlerSetting< T > > handleExceptionsFor(const std::shared_ptr< IEventHandler< T > >& eventHandler)
        {
            return std::make_shared< ExceptionHandlerSetting< T > >(eventHandler, m_consumerRepository);
        }

        /// <summary>
        /// Create a group of event handlers to be used as a dependency.
        /// For example if the handler <code>A</code> must process events before handler <code>B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="handlers">the event handlers, previously set up with <see cref="HandleEventsWith(Disruptor.IEventHandler{T}[])"/>,
        /// that will form the barrier for subsequent handlers or processors.</param>
        /// <returns>an <see cref="EventHandlerGroup{T}"/> that can be used to setup a dependency barrier over the specified event handlers.</returns>
        std::shared_ptr< EventHandlerGroupType > after(const std::vector< std::shared_ptr< IEventHandler< T > > >& handlers)
        {
            std::vector< std::shared_ptr< ISequence > > sequences;
            for (auto&& handler : handlers)
                sequences.push_back(m_consumerRepository->getSequenceFor(handler));

            return std::make_shared< EventHandlerGroupType >(this->shared_from_this(), m_consumerRepository, sequences);
        }

        /// <summary>
        /// Create a group of event handlers to be used as a dependency.
        /// For example if the handler <code>A</code> must process events before handler <code>B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="handler">the event handler, previously set up with <see cref="HandleEventsWith(Disruptor.IEventHandler{T}[])"/>,
        /// that will form the barrier for subsequent handlers or processors.</param>
        /// <returns>an <see cref="EventHandlerGroup{T}"/> that can be used to setup a dependency barrier over the specified event handlers.</returns>
        std::shared_ptr< EventHandlerGroupType > after(const std::shared_ptr< IEventHandler< T > >& handler)
        {
            return after(std::vector< std::shared_ptr< IEventHandler< T > > > { handler });
        }

        /// <summary>
        /// Create a group of event processors to be used as a dependency.
        /// </summary>
        /// <see cref="After(Disruptor.IEventHandler{T}[])"/>
        /// <param name="processors">the event processors, previously set up with <see cref="HandleEventsWith(Disruptor.IEventHandler{T}[])"/>,
        /// that will form the barrier for subsequent handlers or processors.</param>
        /// <returns>an <see cref="EventHandlerGroup{T}"/> that can be used to setup a <see cref="ISequenceBarrier"/> over the specified event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > after(const std::vector< std::shared_ptr< IEventProcessor > >& processors)
        {
            for (auto&& processor : processors)
            {
                m_consumerRepository->add(processor);
            }

            return std::make_shared< EventHandlerGroupType >(this->shared_from_this(), m_consumerRepository, Util::getSequencesFor(processors));
        }

        /// <summary>
        /// Create a group of event processors to be used as a dependency.
        /// </summary>
        /// <see cref="After(Disruptor.IEventHandler{T}[])"/>
        /// <param name="processor">the event processor, previously set up with <see cref="HandleEventsWith(Disruptor.IEventHandler{T}[])"/>,
        /// that will form the barrier for subsequent handlers or processors.</param>
        /// <returns>an <see cref="EventHandlerGroup{T}"/> that can be used to setup a <see cref="ISequenceBarrier"/> over the specified event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > after(const std::shared_ptr< IEventProcessor >& processor)
        {
            return after(std::vector< std::shared_ptr< IEventProcessor > > { processor });
        }

        /// <summary>
        /// Publish an event to the ring buffer.
        /// </summary>
        /// <param name="eventTranslator">the translator that will load data into the event</param>
        void publishEvent(const std::shared_ptr< IEventTranslator< T > >& eventTranslator)
        {
            m_ringBuffer->publishEvent(eventTranslator);
        }

        /// <summary>
        /// Starts the event processors and returns the fully configured ring buffer.
        /// 
        /// The ring buffer is set up to prevent overwriting any entry that is yet to
        /// be processed by the slowest event processor.
        /// 
        /// This method must only be called once after all event processors have been added.
        /// </summary>
        /// <returns>the configured ring buffer</returns>
        std::shared_ptr< RingBuffer< T > > start()
        {
            m_ringBuffer->addGatingSequences(m_consumerRepository->getLastSequenceInChain(true));

            checkOnlyStartedOnce();
            for (auto&& consumerInfo : *m_consumerRepository)
            {
                consumerInfo->start(m_executor);
            }

            return m_ringBuffer;
        }

        /// <summary>
        /// Calls <see cref="IEventProcessor.Halt"/> on all of the event processors created via this disruptor.
        /// </summary>
        void halt()
        {
            for (auto&& consumerInfo : *m_consumerRepository)
            {
                consumerInfo->halt();
            }
        }

        /// <summary>
        /// Waits until all events currently in the disruptor have been processed by all event processors
        /// and then halts the processors.It is critical that publishing to the ring buffer has stopped
        /// before calling this method, otherwise it may never return.
        /// 
        /// This method will not shutdown the executor, nor will it await the final termination of the
        /// processor threads
        /// </summary>
        void shutdown()
        {
            try
            {
                shutdown(ClockConfig::Duration::max());
            }
            catch (TimeoutException& ex)
            {
                m_exceptionHandler->handleOnShutdownException(ex);
            }
        }

        /// <summary>
        /// Waits until all events currently in the disruptor have been processed by all event processors
        /// and then halts the processors.
        /// 
        /// This method will not shutdown the executor, nor will it await the final termination of the
        /// processor threads
        /// </summary>
        /// <param name="timeout">the amount of time to wait for all events to be processed. <code>TimeSpan.MaxValue</code> will give an infinite timeout</param>
        void shutdown(ClockConfig::Duration timeout)
        {
            const auto waitInfinitely = timeout == ClockConfig::Duration::max();

            const auto timeoutAt = ClockConfig::Clock::now() + timeout;
            while (hasBacklog())
            {
                if (!waitInfinitely && timeout.count() >= 0 && ClockConfig::Clock::now() > timeoutAt)
                    DISRUPTOR_THROW_TIMEOUT_EXCEPTION();

                std::this_thread::yield();
            }
            halt();
        }

        /// <summary>
        /// The <see cref="RingBuffer{T}"/> used by this Disruptor. This is useful for creating custom
        /// event processors if the behaviour of <see cref="BatchEventProcessor{T}"/> is not suitable.
        /// </summary>
        std::shared_ptr< RingBuffer< T > > ringBuffer() const
        {
            return m_ringBuffer;
        }

        /// <summary>
        /// Get the value of the cursor indicating the published sequence.
        /// </summary>
        std::int64_t cursor() const
        {
            return m_ringBuffer->cursor();
        }

        /// <summary>
        /// The capacity of the data structure to hold entries.
        /// </summary>
        std::int64_t bufferSize() const
        {
            return m_ringBuffer->bufferSize();
        }

        /// <summary>
        /// Get the event for a given sequence in the RingBuffer.
        /// <see cref="RingBuffer{T}.this"/>
        /// </summary>
        /// <param name="sequence">sequence for the event</param>
        /// <returns>event for the sequence</returns>
        T& operator[](std::int64_t sequence) const
        {
            return m_ringBuffer[sequence];
        }

        /// <summary>
        /// Get the <see cref="ISequenceBarrier"/> used by a specific handler. Note that the <see cref="ISequenceBarrier"/>
        /// may be shared by multiple event handlers.
        /// </summary>
        /// <param name="handler">the handler to get the barrier for</param>
        /// <returns>the SequenceBarrier used by the given handler</returns>
        std::shared_ptr< ISequenceBarrier > getBarrierFor(const std::shared_ptr< IEventHandler< T > >& handler)
        {
            return m_consumerRepository->getBarrierFor(handler);
        }

        std::shared_ptr< EventHandlerGroupType > createEventProcessors(const std::vector< std::shared_ptr< ISequence > >& barrierSequences,
                                                                       const std::vector< std::shared_ptr< IEventHandler< T > > >& eventHandlers)
        {
            checkNotStarted();

            std::vector< std::shared_ptr< ISequence > > processorSequences;
            processorSequences.reserve(eventHandlers.size());

            auto barrier = m_ringBuffer->newBarrier(barrierSequences);

            for (auto&& eventHandler : eventHandlers)
            {
                auto batchEventProcessor = std::make_shared< BatchEventProcessor< T > >(m_ringBuffer, barrier, eventHandler);
                if (m_exceptionHandler != nullptr)
                    batchEventProcessor->setExceptionHandler(m_exceptionHandler);

                m_consumerRepository->add(batchEventProcessor, eventHandler, barrier);
                processorSequences.push_back(batchEventProcessor->sequence());
            }

            if (!processorSequences.empty())
            {
                m_consumerRepository->unMarkEventProcessorsAsEndOfChain(barrierSequences);
            }

            return std::make_shared< EventHandlerGroupType >(this->shared_from_this(), m_consumerRepository, processorSequences);
        }

        std::shared_ptr< EventHandlerGroupType > createWorkerPool(const std::vector< std::shared_ptr< ISequence > >& barrierSequences,
                                                                  const std::vector< std::shared_ptr< IWorkHandler< T > > >& workHandlers)
        {
            auto sequenceBarrier = m_ringBuffer->newBarrier(barrierSequences);
            auto workerPool = std::make_shared< WorkerPool< T > >(m_ringBuffer, sequenceBarrier, m_exceptionHandler, workHandlers);
            m_consumerRepository->add(workerPool, sequenceBarrier);

            return std::make_shared< EventHandlerGroupType >(this->shared_from_this(), m_consumerRepository, workerPool->getWorkerSequences());
        }

        std::shared_ptr< EventHandlerGroupType > createEventProcessors(const std::vector< std::shared_ptr< ISequence > >& barrierSequences,
                                                                       const std::vector< std::shared_ptr< IEventProcessorFactory< T > > >& processorFactories)
        {
            std::vector< std::shared_ptr< IEventProcessor > > processors;
            for (auto&& processorFactory : processorFactories)
            {
                processors.push_back(processorFactory->createEventProcessor(m_ringBuffer, barrierSequences));
            }

            return handleEventsWith(processors);
        }

    private:
        /// <summary>
        /// Private constructor helper
        /// </summary>
        disruptor(const std::shared_ptr< RingBuffer< T > >& ringBuffer, const std::shared_ptr< IExecutor >& executor)
            : m_ringBuffer(ringBuffer)
            , m_executor(executor)
        {
        }

        // Confirms if all messages have been consumed by all event processors
        bool hasBacklog()
        {
            auto cursor = m_ringBuffer->cursor();
            for (auto&& sequence : m_consumerRepository->getLastSequenceInChain(false))
            {
                if (cursor > sequence->value())
                    return true;
            }
            return false;
        }

        void checkNotStarted() const
        {
            if (m_started == 1)
            {
                DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("All event handlers must be added before calling starts.");
            }
        }

        void checkOnlyStartedOnce()
        {
            if (std::atomic_exchange(&m_started, 1) != 0)
            {
                DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("Disruptor.start() must only be called once.");
            }
        }

        std::shared_ptr< RingBuffer< T > > m_ringBuffer;
        std::shared_ptr< IExecutor > m_executor;
        std::shared_ptr< ConsumerRepository< T > > m_consumerRepository = std::make_shared< ConsumerRepository< T > >();
        std::shared_ptr< IExceptionHandler< T > > m_exceptionHandler = std::make_shared< ExceptionHandlerWrapper< T > >();
        std::atomic< int > m_started {0};
    };

} // namespace Disruptor
