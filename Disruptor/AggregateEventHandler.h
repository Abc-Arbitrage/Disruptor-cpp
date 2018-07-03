#pragma once

#include <memory>
#include <vector>

#include "Disruptor/IEventHandler.h"
#include "Disruptor/ILifecycleAware.h"


namespace Disruptor
{

    /**
     * An aggregate collection of IEventHandler<T> that get called in sequence for each event.
     * \tparam T event implementation storing the data for sharing during exchange or parallel coordination of an event
     */ 
    template <class T>
    class AggregateEventHandler : public IEventHandler< T >, public ILifecycleAware
    {
    public:
        /**
         * Construct an aggregate collection of IEventHandler<T>/> to be called in sequence.
         * 
         * \param eventHandlers to be called in sequence
         */ 
        explicit AggregateEventHandler(const std::vector< std::shared_ptr< IEventHandler< T > > >& eventHandlers = {})
            : m_eventHandlers(eventHandlers)
        {
        }

        /**
         * Called when a publisher has committed an event to the RingBuffer<T>
         * 
         * \param data Data committed to the RingBuffer<T>/>
         * \param sequence Sequence number committed to theRingBuffer<T>/>
         * \param endOfBatch flag to indicate if this is the last event in a batch from theRingBuffer<T>/>
         */ 
        void onEvent(T& data, std::int64_t sequence, bool endOfBatch) override
        {
            for (auto i = 0u; i < m_eventHandlers.size(); ++i)
            {
                m_eventHandlers[i]->onEvent(data, sequence, endOfBatch);
            }
        }

        /**
         * Called once on thread start before first event is available.
         */ 
        void onStart() override
        {
            for (auto&& eventHandler : m_eventHandlers)
            {
                auto handler = std::dynamic_pointer_cast< ILifecycleAware >(eventHandler);
                if (handler != nullptr)
                    handler->onStart();
            }
        }

        /**
         * Called once just before the thread is shutdown.
         */ 
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
