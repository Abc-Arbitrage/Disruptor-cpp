#pragma once

#include "Disruptor/IEventHandler.h"
#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /**
     * Used by the BatchEventProcessor<T> to set a callback allowing the IEventHandler<T> to notify when it has finished consuming an event if this happens after the IEventHandler<T>.onEvent() call.
     * Typically this would be used when the handler is performing some sort of batching operation such as writing to an IO device; after the operation has completed, 
     * the implementation should set Sequence.value to update the sequence and allow other processes that are dependent on this handler to progress.
     * 
     * \tparam T event implementation storing the data for sharing during exchange or parallel coordination of an event.
     */ 
    template <class T>
    class ISequenceReportingEventHandler : public IEventHandler< T >
    {
    public:
        virtual ~ISequenceReportingEventHandler() = default;

        /**
         * Call by the BatchEventProcessor<T> to setup the callback.
         * 
         * \param sequenceCallback callback on which to notify the BatchEventProcessor<T> that the sequence has progressed.
         */ 
        virtual void setSequenceCallback(const std::shared_ptr< ISequence >& sequenceCallback) = 0;
    };

} // namespace Disruptor
