#include "stdafx.h"
#include "ManualResetEvent.h"


namespace Disruptor
{
namespace Tests
{

    ManualResetEvent::ManualResetEvent(bool signaled)
        : ResetEvent(signaled)
    {
    }

    void ManualResetEvent::set(bool ensureWaitIsTriggered)
    {
        ResetEvent::set(ensureWaitIsTriggered);
    }

    void ManualResetEvent::reset()
    {
        ResetEvent::reset();
    }

    bool ManualResetEvent::isSet() const
    {
        return ResetEvent::isSet();
    }

    bool ManualResetEvent::waitOne()
    {
        return wait(std::chrono::hours());
    }

    bool ManualResetEvent::wait(ClockConfig::Duration timeDuration)
    {
        return ResetEvent::wait(timeDuration);
    }

} // namespace Tests
} // namespace Disruptor
