#pragma once

#include <memory>
#include <vector>

#include "Disruptor/IExecutor.h"
#include "Disruptor/ISequence.h"
#include "Disruptor/ISequenceBarrier.h"


namespace Disruptor
{

    class IConsumerInfo
    {
    public:
        virtual ~IConsumerInfo() = default;

        virtual std::vector< std::shared_ptr< ISequence > > sequences() const = 0;
        
        virtual const std::shared_ptr< ISequenceBarrier >& barrier() const = 0;

        virtual bool isEndOfChain() const = 0;

        virtual void start(const std::shared_ptr< IExecutor >& executor) = 0;
        virtual void halt() = 0;

        virtual void markAsUsedInBarrier() = 0;

        virtual bool isRunning() const = 0;
    };

} // namespace Disruptor
