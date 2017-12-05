#pragma once

#include <memory>
#include <vector>

#include "Disruptor/ConsumerRepository.h"
#include "Disruptor/IEventHandler.h"
#include "Disruptor/IEventProcessorFactory.h"
#include "Disruptor/ISequence.h"


namespace Disruptor
{

    ///<summary>
    /// A group of <see cref="IEventProcessor"/>s used as part of the <see cref="Disruptor"/>
    ///</summary>
    ///<typeparam name="T">the type of event used by <see cref="IEventProcessor"/>s.</typeparam>
    template
    <
        class T,
        template <class> class TDisruptor
    >
    class EventHandlerGroup
    {
        typedef EventHandlerGroup< T, TDisruptor > EventHandlerGroupType;

    public:
        EventHandlerGroup(const std::shared_ptr< TDisruptor< T > >& disruptor,
                          const std::shared_ptr< ConsumerRepository< T > >& consumerRepository,
                          const std::vector< std::shared_ptr< ISequence > >& sequences)
            : m_disruptor(disruptor)
            , m_consumerRepository(consumerRepository)
            , m_sequences(sequences)
        {
        }

        /// <summary>
        /// Create a new event handler group that combines the consumers in this group with <paramref name="otherHandlerGroup"/>
        /// </summary>
        /// <param name="otherHandlerGroup">the event handler group to combine</param>
        /// <returns>a new EventHandlerGroup combining the existing and new consumers into a single dependency group</returns>
        std::shared_ptr< EventHandlerGroupType > And(const std::shared_ptr< EventHandlerGroupType >& otherHandlerGroup)
        {
            std::vector< std::shared_ptr< ISequence > > sequences(m_sequences);
            std::copy(otherHandlerGroup->m_sequences.begin(), otherHandlerGroup->m_sequences.end(), std::back_inserter(sequences));

            return std::make_shared< EventHandlerGroupType >(m_disruptor, m_consumerRepository, sequences);
        }
        
        /// <summary>
        /// Create a new event handler group that combines the handlers in this group with <paramref name="processors"/>.
        /// </summary>
        /// <param name="processors">the processors to combine</param>
        /// <returns>a new EventHandlerGroup combining the existing and new processors into a single dependency group</returns>
        std::shared_ptr< EventHandlerGroupType > And(const std::vector< std::shared_ptr< IEventProcessor > >& processors)
        {
            std::vector< std::shared_ptr< ISequence > > sequences;
            
            for (auto&& eventProcessor : processors)
            {
                m_consumerRepository->add(eventProcessor);
                sequences.push_back(eventProcessor->sequence());
            }

            std::copy(m_sequences.begin(), m_sequences.end(), std::back_inserter(sequences));

            return std::make_shared< EventHandlerGroupType >(m_disruptor, m_consumerRepository, sequences);
        }
        
        /// <summary>
        /// Create a new event handler group that combines the handlers in this group with <paramref name="processors"/>.
        /// </summary>
        /// <param name="processor">the processor to combine</param>
        /// <returns>a new EventHandlerGroup combining the existing and new processors into a single dependency group</returns>
        std::shared_ptr< EventHandlerGroupType > And(const std::shared_ptr< IEventProcessor >& processor)
        {
            return And(std::vector< std::shared_ptr< IEventProcessor > > { processor });
        }

        /// <summary>
        /// Set up batch handlers to consume events from the ring buffer. These handlers will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).then(B);</code>
        /// </summary>
        /// <param name="handlers"></param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > then(const std::vector< std::shared_ptr< IEventHandler< T > > >& handlers)
        {
            return handleEventsWith(handlers);
        }

        /// <summary>
        /// Set up batch handlers to consume events from the ring buffer. These handlers will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// <code>dw.HandleEventsWith(A).then(B);</code>
        /// </summary>
        /// <param name="handler"></param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > then(const std::shared_ptr< IEventHandler< T > >& handler)
        {
            return then(std::vector< std::shared_ptr< IEventHandler< T > > > { handler });
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Disruptor{T}.Start"/> is called.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// </summary>
        /// <param name="eventProcessorFactories">the event processor factories to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > then(const std::vector< std::shared_ptr< IEventProcessorFactory< T > > >& eventProcessorFactories)
        {
            return handleEventsWith(eventProcessorFactories);
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Disruptor{T}.Start"/> is called.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before handler<code>B</code>:
        /// </summary>
        /// <param name="eventProcessorFactory">the event processor factory to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > then(const std::shared_ptr< IEventProcessorFactory< T > >& eventProcessorFactory)
        {
            return then(std::vector< std::shared_ptr< IEventProcessorFactory< T > > > { eventProcessorFactory });
        }

        /// <summary>
        /// Set up a worker pool to handle events from the ring buffer. The worker pool will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event. Each event will be processed
        /// by one of the work handler instances.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before the worker pool with handlers <code>B, C</code>:
        /// <code>dw.HandleEventsWith(A).ThenHandleEventsWithWorkerPool(B, C);</code>
        /// </summary>
        /// <param name="handlers">the work handlers that will process events. Each work handler instance will provide an extra thread in the worker pool.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to set up a event processor barrier over the created event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > thenHandleEventsWithWorkerPool(const std::vector< std::shared_ptr< IWorkHandler< T > > >& handlers)
        {
            return handleEventsWithWorkerPool(handlers);
        }

        /// <summary>
        /// Set up batch handlers to handle events from the ring buffer. These handlers will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event.
        /// 
        /// This method is generally used as part of a chain. For example if <code>A</code> must
        /// process events before<code> B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="handlers">the batch handlers that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to set up a event processor barrier over the created event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::vector< std::shared_ptr< IEventHandler< T > > >& handlers)
        {
            return m_disruptor->createEventProcessors(m_sequences, handlers);
        }

        /// <summary>
        /// Set up batch handlers to handle events from the ring buffer. These handlers will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event.
        /// 
        /// This method is generally used as part of a chain. For example if <code>A</code> must
        /// process events before<code> B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="handler">the batch handler that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to set up a event processor barrier over the created event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::shared_ptr< IEventHandler< T > >& handler)
        {
            return handleEventsWith(std::vector< std::shared_ptr< IEventHandler< T > > > { handler });
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Disruptor{T}.Start"/> is called.
        /// 
        /// This method is generally used as part of a chain. For example if <code>A</code> must
        /// process events before<code> B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="eventProcessorFactories">the event processor factories to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::vector< std::shared_ptr< IEventProcessorFactory< T > > >& eventProcessorFactories)
        {
            return m_disruptor->createEventProcessors(m_sequences, eventProcessorFactories);
        }

        /// <summary>
        /// Set up custom event processors to handle events from the ring buffer. The Disruptor will
        /// automatically start these processors when <see cref="Disruptor{T}.Start"/> is called.
        /// 
        /// This method is generally used as part of a chain. For example if <code>A</code> must
        /// process events before<code> B</code>:
        /// <code>dw.After(A).HandleEventsWith(B);</code>
        /// </summary>
        /// <param name="eventProcessorFactory">the event processor factory to use to create the event processors that will process events.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to chain dependencies.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWith(const std::shared_ptr< IEventProcessorFactory< T > >& eventProcessorFactory)
        {
            return handleEventsWith(std::vector< std::shared_ptr< IEventProcessorFactory< T > > > { eventProcessorFactory });
        }

        /// <summary>
        /// Set up a worker pool to handle events from the ring buffer. The worker pool will only process events
        /// after every <see cref="IEventProcessor"/> in this group has processed the event. Each event will be processed
        /// by one of the work handler instances.
        /// 
        /// This method is generally used as part of a chain. For example if the handler <code>A</code> must
        /// process events before the worker pool with handlers <code>B, C</code>:
        /// <code>dw.After(A).HandleEventsWithWorkerPool(B, C);</code>
        /// </summary>
        /// <param name="handlers">the work handlers that will process events. Each work handler instance will provide an extra thread in the worker pool.</param>
        /// <returns>a <see cref="EventHandlerGroup{T}"/> that can be used to set up a event processor barrier over the created event processors.</returns>
        std::shared_ptr< EventHandlerGroupType > handleEventsWithWorkerPool(const std::vector< std::shared_ptr< IWorkHandler< T > > >& handlers)
        {
            return m_disruptor->createWorkerPool(m_sequences, handlers);
        }

        /// <summary>
        /// Create a dependency barrier for the processors in this group.
        /// This allows custom event processors to have dependencies on
        /// <see cref="BatchEventProcessor{T}"/>s created by the disruptor.
        /// </summary>
        /// <returns>a <see cref="ISequenceBarrier"/> including all the processors in this group.</returns>
        std::shared_ptr< ISequenceBarrier > asSequenceBarrier()
        {
            return m_disruptor->ringBuffer()->newBarrier(m_sequences);
        }

    private:
        std::shared_ptr< TDisruptor< T > > m_disruptor;
        std::shared_ptr< ConsumerRepository< T > > m_consumerRepository;
        std::vector< std::shared_ptr< ISequence > > m_sequences;
    };

} // namespace Disruptor
