#pragma once

#include <memory>

#include "Disruptor/IEventProcessor.h"
#include "Disruptor/RingBuffer.h"


namespace Disruptor
{

    /**
     * A factory interface to make it possible to include custom event processors in a chain
     */ 
    template <class T>
    class IEventProcessorFactory
    {
    public:
        virtual ~IEventProcessorFactory() = default;

        /**
         * Create a new event processor that gates on barrierSequences
         *
         * \param ringBuffer ring buffer
         * \param barrierSequences barrierSequences the sequences to gate on
         * \returns a new EventProcessor that gates on before processing events
         */ 
        virtual std::shared_ptr< IEventProcessor > createEventProcessor(const std::shared_ptr< RingBuffer< T > >& ringBuffer,
                                                                        const std::vector< std::shared_ptr< ISequence > >& barrierSequences) = 0;
    };

} // namespace Disruptor
