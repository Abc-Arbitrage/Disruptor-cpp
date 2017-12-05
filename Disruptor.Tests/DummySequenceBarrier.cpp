#include "stdafx.h"
#include "DummySequenceBarrier.h"


namespace Disruptor
{
namespace Tests
{

    std::int64_t DummySequenceBarrier::waitFor(std::int64_t)
    {
        return 0;
    }

    std::int64_t DummySequenceBarrier::cursor()
    {
        return 0;
    }

    bool DummySequenceBarrier::isAlerted()
    {
        return false;
    }

    void DummySequenceBarrier::alert()
    {
    }

    void DummySequenceBarrier::clearAlert()
    {
    }

    void DummySequenceBarrier::checkAlert()
    {
    }

} // namespace Tests
} // namespace Disruptor
