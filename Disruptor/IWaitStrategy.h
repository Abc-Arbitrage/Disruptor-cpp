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

        /// <summary>
        /// Wait for the given sequence to be available.  It is possible for this method to return a value
        /// less than the sequence number supplied depending on the implementation of the WaitStrategy.A common
        /// use for this is to signal a timeout.Any EventProcessor that is using a WaitStrategy to get notifications
        /// about message becoming available should remember to handle this case.  The <see cref="BatchEventProcessor{T}"/>
        /// explicitly handles this case and will signal a timeout if required.
        /// </summary>
        /// <param name="sequence">sequence to be waited on.</param>
        /// <param name="cursor">Ring buffer cursor on which to wait.</param>
        /// <param name="dependentSequence">on which to wait.</param>
        /// <param name="barrier">barrier the <see cref="IEventProcessor"/> is waiting on.</param>
        /// <returns>the sequence that is available which may be greater than the requested sequence.</returns>
        virtual std::int64_t waitFor(std::int64_t sequence,
                                     const std::shared_ptr< Sequence >& cursor,
                                     const std::shared_ptr< ISequence >& dependentSequence,
                                     const std::shared_ptr< ISequenceBarrier >& barrier) = 0;

        /// <summary>
        /// Signal those <see cref="IEventProcessor"/> waiting that the cursor has advanced.
        /// </summary>
        virtual void signalAllWhenBlocking() = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
