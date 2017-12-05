#pragma once

namespace Disruptor
{

    /// <summary>
    /// Callback interface to be implemented for processing units of work as they become available in the <see cref="RingBuffer{T}"/>
    /// 
    /// </summary>
    /// <typeparam name="T">event implementation storing the data for sharing during exchange or parallel coordination of an event.</typeparam>
    template <class T>
    class IWorkHandler
    {
    public:
        virtual ~IWorkHandler() = default;

        /// <summary>
        /// Callback to indicate a unit of work needs to be processed.
        /// </summary>
        /// <param name="evt">event published to the <see cref="RingBuffer{T}"/></param>
        virtual void onEvent(T& evt) = 0;
    };

} // namespace Disruptor
