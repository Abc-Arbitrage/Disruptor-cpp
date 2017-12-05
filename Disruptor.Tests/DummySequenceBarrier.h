#pragma once

#include "Disruptor/ISequenceBarrier.h"


namespace Disruptor
{
namespace Tests
{

    class DummySequenceBarrier : public ISequenceBarrier
    {
    public:
        std::int64_t waitFor(std::int64_t sequence) override;

        std::int64_t cursor() override;

        bool isAlerted() override;
        void alert() override;

        void clearAlert() override;
        void checkAlert() override;
    };

} // namespace Tests
} // namespace Disruptor
