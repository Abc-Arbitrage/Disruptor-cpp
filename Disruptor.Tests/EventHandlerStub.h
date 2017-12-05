#pragma once

#include <memory>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.TestTools/CountdownEvent.h"


namespace Disruptor
{
namespace Tests
{
    
    template <class T>
    class EventHandlerStub : public IEventHandler< T >
    {
    public:
        explicit EventHandlerStub(const std::shared_ptr< CountdownEvent >& countDownLatch)
            : m_countDownLatch(countDownLatch)
        {
        }

        void onEvent(T& /*data*/, std::int64_t /*sequence*/, bool /*endOfBatch*/) override
        {
            m_countDownLatch->signal();
        }

    private:
        std::shared_ptr< CountdownEvent > m_countDownLatch;
    };

} // namespace Tests
} // namespace Disruptor
