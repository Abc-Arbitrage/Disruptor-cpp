#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>


namespace Disruptor
{

    class ISequence;
    class ISequenceBarrier;
    class Sequence;


    class IWaitStrategy
    {
    public:
        virtual ~IWaitStrategy() = default;

        /**
         * Wait for the given sequence to be available.  It is possible for this method to return a value less than the sequence number supplied depending on the implementation of the WaitStrategy. 
         * A common use for this is to signal a timeout.Any EventProcessor that is using a WaitStrategy to get notifications about message becoming available should remember to handle this case. 
         * The BatchEventProcessor<T> explicitly handles this case and will signal a timeout if required.
         * 
         * \param sequence sequence to be waited on.
         * \param cursor Ring buffer cursor on which to wait.
         * \param dependentSequence on which to wait.
         * \param barrier barrier the IEventProcessor is waiting on.
         * \returns the sequence that is available which may be greater than the requested sequence.
         */ 
        virtual std::int64_t waitFor(std::int64_t sequence,
                                     Sequence& cursor,
                                     ISequence& dependentSequence,
                                     ISequenceBarrier& barrier) = 0;

        /**
         * Signal those IEventProcessor waiting that the cursor has advanced.
         */ 
        virtual void signalAllWhenBlocking() = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
