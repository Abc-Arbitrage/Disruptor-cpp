#pragma once

#include <cstdint>
#include <string>

#include "Disruptor/IEventTranslatorVararg.h"


namespace Disruptor
{
namespace Tests
{

    class StubEvent
    {
        typedef IEventTranslatorVararg< StubEvent, std::int32_t, std::string > EventTranslatorType;

        class TwoTranslator : public EventTranslatorType
        {
        public:
            void translateTo(StubEvent& event, std::int64_t sequence, const std::int32_t& arg0, const std::string& arg1) override;
        };

    public:
        StubEvent()
            : m_value(0)
            , m_testString()
        {}

        explicit StubEvent(int i);

        void operator=(const StubEvent& other)
        {
            m_value = other.m_value;
            m_testString = other.m_testString;
        }

        int value() const;
        void value(int i);

        std::string testString() const;
        void testString(std::string x);

        void copy(const StubEvent& evt);

        bool operator==(const StubEvent& rhs) const;

        static const std::shared_ptr< TwoTranslator >& translator();

        static const std::function< StubEvent() >& eventFactory();

    private:
        int m_value;
        std::string m_testString;
    };
    
} // namespace Tests
} // namespace Disruptor


#include <ostream>


namespace std
{

    ostream& operator<<(ostream& stream, const Disruptor::Tests::StubEvent& event);

} // namespace std
