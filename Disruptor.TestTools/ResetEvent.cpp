#include "stdafx.h"
#include "ResetEvent.h"


namespace Disruptor
{
namespace Tests
{

    ResetEvent::ResetEvent(bool signaled)
        : m_signaled(signaled)
        , m_mutex()
        , m_conditionVariable()
        , m_waitCount(0)
    {
    }

    void ResetEvent::set(bool ensureWaitIsTriggered)
    {
        {
            std::unique_lock< decltype(m_mutex) > lock(m_mutex);
            m_signaled = true;
        }
        m_conditionVariable.notify_all();

        while (ensureWaitIsTriggered && m_waitCount > 0)
            std::this_thread::yield();
    }

    void ResetEvent::reset()
    {
        std::unique_lock< decltype(m_mutex) > lock(m_mutex);
        m_signaled = false;
    }

    bool ResetEvent::internalWait(ClockConfig::Duration timeDuration)
    {
        std::unique_lock< decltype(m_mutex) > lock(m_mutex);

        auto hasReceivedSignal = false;

        if (!m_signaled)
        {
            ++m_waitCount;

            if (timeDuration == ClockConfig::Duration())
            {
                do
                {
                    m_conditionVariable.wait(lock);
                }
                while (!m_signaled);

                hasReceivedSignal = true;
            }
            else
            {
                hasReceivedSignal = m_conditionVariable.wait_for(lock, timeDuration, [this] { return this->m_signaled; });
            }

            --m_waitCount;
        }

        return hasReceivedSignal || m_signaled;
    }

    bool ResetEvent::wait(ClockConfig::Duration timeDuration)
    {
        return internalWait(timeDuration);
    }

    bool ResetEvent::isSet() const
    {
        std::lock_guard< decltype(m_mutex) > lock(m_mutex);
        return m_signaled;
    }

} // namespace Tests
} // namespace Disruptor
