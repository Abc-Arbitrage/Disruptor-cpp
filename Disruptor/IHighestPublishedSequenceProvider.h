#pragma once

#include <cstdint>

namespace Disruptor
{

    class IHighestPublishedSequenceProvider
    {
    public:
        virtual ~IHighestPublishedSequenceProvider() = default;

        /**
         * Get the highest sequence number that can be safely read from the ring buffer. Depending on the implementation of the Sequencer this call may need to scan a number of values
         * in the Sequencer. The scan will range from nextSequence to availableSequence. If there are no available values > nextSequence() the return value will be nextSequence - 1
         * To work correctly a consumer should pass a value that it 1 higher than the last sequence that was successfully processed.
         *
         * \param nextSequence The sequence to start scanning from.
         * \param availableSequence The sequence to scan to.
         * \returns The highest value that can be safely read, will be at least nextSequence - 1
         */
        virtual std::int64_t getHighestPublishedSequence(std::int64_t nextSequence, std::int64_t availableSequence) = 0;
    };

} // namespace Disruptor
