#pragma once

#include <memory>
#include <initializer_list>
#include <iosfwd>

#include "Disruptor/ICursored.h"
#include "Disruptor/IHighestPublishedSequenceProvider.h"
#include "Disruptor/ISequenced.h"


namespace Disruptor
{

    template <class T> class EventPoller;
    template <class T> class IDataProvider;
    class ISequence;
    class ISequenceBarrier;


    /// <summary>
    /// Coordinator for claiming sequences for access to a data structure while tracking dependent <see cref="Sequence"/>s
    /// </summary>
    template <class T>
    class ISequencer : public ISequenced, public ICursored, public IHighestPublishedSequenceProvider
    {
    public:
        virtual ~ISequencer() = default;

        /// <summary>
        /// Claim a specific sequence when only one publisher is involved.
        /// </summary>
        /// <param name="sequence">sequence to be claimed.</param>
        virtual void claim(std::int64_t sequence) = 0;

        /// <summary>
        /// Confirms if a sequence is published and the event is available for use; non-blocking.
        /// </summary>
        /// <param name="sequence">sequence of the buffer to check</param>
        /// <returns>true if the sequence is available for use, false if not</returns>
        virtual bool isAvailable(std::int64_t sequence) = 0;

        /// <summary>
        /// Add the specified gating sequences to this instance of the Disruptor.  They will
        /// safely and atomically added to the list of gating sequences. 
        /// </summary>
        /// <param name="gatingSequences">The sequences to add.</param>
        virtual void addGatingSequences(const std::vector< std::shared_ptr< ISequence > >& gatingSequences) = 0;

        /// <summary>
        /// Remove the specified sequence from this sequencer.
        /// </summary>
        /// <param name="sequence">to be removed.</param>
        /// <returns>true if this sequence was found, false otherwise.</returns>
        virtual bool removeGatingSequence(const std::shared_ptr< ISequence >& sequence) = 0;

        /// <summary>
        /// Create a <see cref="ISequenceBarrier"/> that gates on the the cursor and a list of <see cref="Sequence"/>s
        /// </summary>
        /// <param name="sequencesToTrack"></param>
        /// <returns></returns>
        virtual std::shared_ptr< ISequenceBarrier > newBarrier(const std::vector< std::shared_ptr< ISequence > >& sequencesToTrack) = 0;

        /// <summary>
        /// Get the minimum sequence value from all of the gating sequences
        /// added to this ringBuffer.
        /// </summary>
        /// <returns>The minimum gating sequence or the cursor sequence if no sequences have been added.</returns>
        virtual std::int64_t getMinimumSequence() = 0;

        virtual std::shared_ptr< EventPoller< T > > newPoller(const std::shared_ptr< IDataProvider< T > >& provider, const std::vector< std::shared_ptr< ISequence > >& gatingSequences) = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
