#pragma once

#include <memory>

#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /**
     * Implement this interface in your event handler to obtain the <see cref="T:Disruptor.IEventProcessor" /> sequence.
     *
     * Used by the <see cref="T:Disruptor.IEventProcessor" /> to set a callback allowing the event handler to notify
     * when it has finished consuming an event if this happens after the OnEvent call.
     *
     * Typically this would be used when the handler is performing some sort of batching operation such as writing to an IO
     * device; after the operation has completed, the implementation should set <see cref="P:Disruptor.Sequence.Value" /> to update the
     * sequence and allow other processes that are dependent on this handler to progress.
     */
    class IEventProcessorSequenceAware
    {
    public:
        virtual ~IEventProcessorSequenceAware() = default;

    	/**
    	 * Call by the <see cref="T:Disruptor.IEventProcessor" /> to setup the callback.
    	 *
    	 * \param sequenceCallback callback on which to notify the <see cref="T:Disruptor.IEventProcessor" /> that the sequence has progressed.
    	 */
        virtual void setSequenceCallback(const std::shared_ptr< ISequence >& sequenceCallback) = 0;
    };

} // namespace Disruptor
