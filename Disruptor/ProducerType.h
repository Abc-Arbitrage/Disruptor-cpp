#pragma once


namespace Disruptor
{
    /**
     * Defines producer types to support creation of RingBuffer with correct sequencer and publisher.
     */ 
    enum class ProducerType
    {
        /**
         * Create a RingBuffer with a single event publisher to the RingBuffer
         */ 
        Single,

        /**
         * Create a RingBuffer supporting multiple event publishers to the one RingBuffer
         */ 
        Multi
    };

} // namespace Disruptor


#include <iosfwd>


namespace std
{

    ostream& operator<<(ostream& stream, const Disruptor::ProducerType& value);

} // namespace std
