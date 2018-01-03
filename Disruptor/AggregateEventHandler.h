#pragma once

#include <memory>
#include <vector>

#include "Disruptor/IEventHandler.h"
#include "Disruptor/ILifecycleAware.h"


namespace Disruptor
{

    /// <summary>
    /// An aggregate collection of <see cref="IEventHandler{T}"/> that get called in sequence for each event.
    /// </summary>
    /// <typeparam name="T">event implementation storing the data for sharing during exchange or parallel coordination of an event</typeparam>
    template <class T>
    class AggregateEventHandler : public IEventHandler< T >, public ILifecycleAware
    {
    public:
        /// <summary>
        /// Construct an aggregate collection of <see cref="IEventHandler{T}"/> to be called in sequence.
        /// </summary>
        /// <param name="eventHandlers">to be called in sequence</param>
        explicit AggregateEventHandler(const std::vector< std::shared_ptr< IEventHandler< T > > >& eventHandlers = {})
            : m_eventHandlers(eventHandlers)
        {
        }

        /// <summary>
        /// Called when a publisher has committed an event to the <see cref="RingBuffer{T}"/>
        /// </summary>
        /// <param name="data">Data committed to the <see cref="RingBuffer{T}"/></param>
        /// <param name="sequence">Sequence number committed to the <see cref="RingBuffer{T}"/></param>
        /// <param name="endOfBatch">flag to indicate if this is the last event in a batch from the <see cref="RingBuffer{T}"/></param>
        void onEvent(T& data, std::int64_t sequence, bool endOfBatch) override
        {
            for (auto i = 0u; i < m_eventHandlers.size(); ++i)
            {
                m_eventHandlers[i]->onEvent(data, sequence, endOfBatch);
            }
        }

        ///<summary>
        /// Called once on thread start before first event is available.
        ///</summary>
        void onStart() override
        {
            for (auto&& eventHandler : m_eventHandlers)
            {
                auto handler = std::dynamic_pointer_cast< ILifecycleAware >(eventHandler);
                if (handler != nullptr)
                    handler->onStart();
            }
        }

        /// <summary>
        /// Called once just before the thread is shutdown.
        /// </summary>
        void onShutdown() override
        {
            for (auto&& eventHandler : m_eventHandlers)
            {
                auto handler = std::dynamic_pointer_cast< ILifecycleAware >(eventHandler);
                if (handler != nullptr)
                    handler->onShutdown();
            }
        }

    private:
        std::vector< std::shared_ptr< IEventHandler< T > > > m_eventHandlers;
    };

} // namespace Disruptor
