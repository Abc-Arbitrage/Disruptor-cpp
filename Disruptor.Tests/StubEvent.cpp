#include "stdafx.h"
#include "StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    void StubEvent::TwoTranslator::translateTo(StubEvent& event, std::int64_t /*sequence*/, const std::int32_t& arg0, const std::string& arg1)
    {
        event.value(arg0);
        event.testString(arg1);
    }

    StubEvent::StubEvent(int i)
        : m_value(i)
    {
    }

    int StubEvent::value() const
    {
        return m_value;
    }

    void StubEvent::value(int i)
    {
        m_value = i;
    }

    std::string StubEvent::testString() const
    {
        return m_testString;
    }

    void StubEvent::testString(std::string x)
    {
        m_testString = x;
    }

    void StubEvent::copy(const StubEvent& evt)
    {
        m_value = evt.m_value;
    }

    bool StubEvent::operator==(const StubEvent& rhs) const
    {
        return m_value == rhs.m_value;
    }

    const std::shared_ptr< StubEvent::TwoTranslator >& StubEvent::translator()
    {
        static auto result(std::make_shared< TwoTranslator >());
        return result;
    }

    const std::function< StubEvent() >& StubEvent::eventFactory()
    {
        static std::function< StubEvent() > result([] { return StubEvent(-1); });
        return result;
    }

} // namespace Tests
} // namespace Disruptor


namespace std
{

    ostream& operator<<(ostream& stream, const Disruptor::Tests::StubEvent& event)
    {
        return stream << "Value: " << event.value() << ", TestString: " << event.testString();
    }

} // namespace std
