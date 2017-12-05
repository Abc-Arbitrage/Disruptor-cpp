#pragma once

#include <cstdint>
#include <memory>


namespace Disruptor
{

    /// <summary>
    /// Translate a data representation into fields set in given event
    /// </summary>
    /// <param name="event">into which the data should be translated.</param>
    /// <param name="sequence">that is assigned to event.</param>
    /// <param name="args">The array of user arguments.</param>
    template <class T, class... TArgs>
    class IEventTranslatorVararg
    {
    public:
        virtual ~IEventTranslatorVararg() = default;

        /// <summary>
        /// Translate a data representation into fields set in given event
        /// </summary>
        /// <param name="eventData">event into which the data should be translated.</param>
        /// <param name="sequence">sequence that is assigned to event.</param>
        virtual void translateTo(T& eventData, std::int64_t sequence, const TArgs&... args) = 0;
    };

} // namespace Disruptor
