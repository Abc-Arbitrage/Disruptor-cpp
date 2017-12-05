#pragma once

#include <cstdint>


namespace Disruptor
{

    /// <summary>
    /// Callback interface to be implemented for processing events as they become available in the <see cref="RingBuffer{T}"/>
    /// </summary>
    /// <typeparam name="T">Type of events for sharing during exchange or parallel coordination of an event.</typeparam>
    /// <remarks>See <see cref="BatchEventProcessor{T}.SetExceptionHandler"/> if you want to handle exceptions propagated out of the handler.</remarks
    template <class T>
    class IEventHandler
    {
    public:
        virtual ~IEventHandler() = default;

        /// <summary>
        /// Called when a publisher has committed an event to the <see cref="RingBuffer{T}"/>
        /// </summary>
        /// <param name="data">Data committed to the <see cref="RingBuffer{T}"/></param>
        /// <param name="sequence">Sequence number committed to the <see cref="RingBuffer{T}"/></param>
        /// <param name="endOfBatch">flag to indicate if this is the last event in a batch from the <see cref="RingBuffer{T}"/></param>
        virtual void onEvent(T& data, std::int64_t sequence, bool endOfBatch) = 0;
    };

} // namespace Disruptor
