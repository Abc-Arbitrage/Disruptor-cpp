#pragma once

#include <memory>

#include "Disruptor/IEventProcessor.h"
#include "Disruptor/RingBuffer.h"


namespace Disruptor
{

    /// <summary>
    /// A factory interface to make it possible to include custom event processors in a chain:
    /// 
    /// <code>disruptor.HandleEventsWith(handler1).Then((ringBuffer, barrierSequences) -> new CustomEventProcessor(ringBuffer, barrierSequences));</code>
    /// </summary>
    template <class T>
    class IEventProcessorFactory
    {
    public:
        virtual ~IEventProcessorFactory() = default;

        /// <summary>
        /// Create a new event processor that gates on <paramref name="barrierSequences"/>
        /// </summary>
        /// <param name="barrierSequences">barrierSequences the sequences to gate on</param>
        /// <returns>a new EventProcessor that gates on <code>barrierSequences</code> before processing events</returns>
        virtual std::shared_ptr< IEventProcessor > createEventProcessor(const std::shared_ptr< RingBuffer< T > >& ringBuffer,
                                                                        const std::vector< std::shared_ptr< ISequence > >& barrierSequences) = 0;
    };

} // namespace Disruptor
