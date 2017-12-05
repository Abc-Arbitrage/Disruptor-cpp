#include "stdafx.h"
#include "CountdownEvent.h"


namespace Disruptor
{
namespace Tests
{

    CountdownEvent::CountdownEvent(std::uint32_t initialCount)
        : ResetEvent(initialCount == 0)
        , m_initialCount(initialCount)
        , m_currentCount(initialCount)
    {}

    std::uint32_t CountdownEvent::initialCount() const
    {
        return m_initialCount;
    }

    std::uint32_t CountdownEvent::currentCount() const
    {
        return m_currentCount;
    }

    void CountdownEvent::reset()
    {
        m_currentCount = initialCount();
        ResetEvent::reset();
    }

    void CountdownEvent::reset(std::uint32_t initialCount)
    {
        m_initialCount = initialCount;
        reset();
    }

    bool CountdownEvent::wait(ClockConfig::Duration timeDuration)
    {
        while (currentCount() != 0)
        {
            ResetEvent::reset();
            ResetEvent::wait(timeDuration);
        }

        return true;
    }

    void CountdownEvent::signal()
    {
        --m_currentCount;
        if (currentCount() == 0)
            set();
    }

} // namespace Tests
} // namespace Disruptor
