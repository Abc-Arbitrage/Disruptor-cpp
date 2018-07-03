#pragma once

#include <cstdint>
#include <memory>


namespace Disruptor
{

    /**
     * Implementations translate (write) data representations into events claimed from the RingBuffer<T>.
     * When publishing to the RingBuffer, provide an EventTranslator. The RingBuffer will select the next available event by sequence and provide 
     * it to the EventTranslator(which should update the event), before publishing the sequence update.
     *
     * \tparam T event implementation storing the data for sharing during exchange or parallel coordination of an event.
     */
    template <class T>
    class IEventTranslator
    {
    public:
        virtual ~IEventTranslator() = default;

        /**
         * Translate a data representation into fields set in given event
         *
         * \param eventData event into which the data should be translated.
         * \param sequence sequence that is assigned to event.
         */ 
        virtual void translateTo(T& eventData, std::int64_t sequence) = 0;
    };

} // namespace Disruptor
