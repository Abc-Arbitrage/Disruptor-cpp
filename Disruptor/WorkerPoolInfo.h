#pragma once

#include <memory>

#include "Disruptor/IConsumerInfo.h"
#include "Disruptor/WorkerPool.h"


namespace Disruptor
{

    template <class T>
    class WorkerPoolInfo : public IConsumerInfo
    {
    public:
        WorkerPoolInfo(const std::shared_ptr< WorkerPool< T > >& workerPool, const std::shared_ptr< ISequenceBarrier >& barrier)
            : m_workerPool(workerPool)
            , m_barrier(barrier)
            , m_isEndOfChain(true)
        {
        }

        std::vector< std::shared_ptr< ISequence > > sequences() const override
        {
            return m_workerPool->getWorkerSequences();
        }

        const std::shared_ptr< ISequenceBarrier >& barrier() const override
        {
            return m_barrier;
        }

        bool isEndOfChain() const override
        {
            return m_isEndOfChain;
        }

        void start(const std::shared_ptr< IExecutor >& executor) override
        {
            m_workerPool->start(executor);
        }

        void halt() override
        {
            m_workerPool->halt();
        }

        void markAsUsedInBarrier() override
        {
            m_isEndOfChain = false;
        }

        bool isRunning() const override
        {
            return m_workerPool->isRunning();
        }
        
    private:
        std::shared_ptr< WorkerPool< T > > m_workerPool;
        std::shared_ptr< ISequenceBarrier > m_barrier;
        bool m_isEndOfChain;
    };

} // namespace Disruptor
