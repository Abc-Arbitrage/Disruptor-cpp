#pragma once

#include "Disruptor/ClockConfig.h"

#include "Disruptor.TestTools/ResetEvent.h"


namespace Disruptor
{
namespace Tests
{

    class ManualResetEvent : public ResetEvent
    {
    public:
        explicit ManualResetEvent(bool signaled = false);

        bool isSet() const override;

        void set(bool ensureWaitIsTriggered = false) override;
        void reset() override;

        bool waitOne();
        bool wait(ClockConfig::Duration timeDuration) override;
    };

} // namespace Tests
} // namespace Disruptor
