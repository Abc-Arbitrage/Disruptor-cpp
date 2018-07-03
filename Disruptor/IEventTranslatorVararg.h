#pragma once

#include <cstdint>

namespace Disruptor
{

    /**
     * Translate a data representation into fields set in given event
     *
     * \param event into which the data should be translated.
     * \param sequence that is assigned to event.
     * \param args The array of user arguments.
     */ 
    template <class T, class... TArgs>
    class IEventTranslatorVararg
    {
    public:
        virtual ~IEventTranslatorVararg() = default;

        /**
         * Translate a data representation into fields set in given event
         *
         * \param eventData event into which the data should be translated.
         * \param sequence sequence that is assigned to event.
         */ 
        virtual void translateTo(T& eventData, std::int64_t sequence, const TArgs&... args) = 0;
    };

} // namespace Disruptor
