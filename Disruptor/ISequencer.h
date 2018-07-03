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


    /**
     * Coordinator for claiming sequences for access to a data structure while tracking dependent Sequences
     */ 
    template <class T>
    class ISequencer : public ISequenced, public ICursored, public IHighestPublishedSequenceProvider
    {
    public:
        virtual ~ISequencer() = default;

        /**
         * Claim a specific sequence when only one publisher is involved.
         * 
         * \param sequence sequence to be claimed.
         */ 
        virtual void claim(std::int64_t sequence) = 0;

        /**
         * Confirms if a sequence is published and the event is available for use; non-blocking.
         * 
         * \param sequence sequence of the buffer to check
         * \returns true if the sequence is available for use, false if not
         */ 
        virtual bool isAvailable(std::int64_t sequence) = 0;

        /**
         * Add the specified gating sequences to this instance of the Disruptor.  They will safely and atomically added to the list of gating sequences.
         * 
         * \param gatingSequences The sequences to add.
         */ 
        virtual void addGatingSequences(const std::vector< std::shared_ptr< ISequence > >& gatingSequences) = 0;

        /**
         * Remove the specified sequence from this sequencer.
         * 
         * \param sequence to be removed.
         * \returns true if this sequence was found, false otherwise.
         */ 
        virtual bool removeGatingSequence(const std::shared_ptr< ISequence >& sequence) = 0;

        /**
         * Create a ISequenceBarrier that gates on the the cursor and a list of Sequences
         * 
         * \param sequencesToTrack 
         */
        virtual std::shared_ptr< ISequenceBarrier > newBarrier(const std::vector< std::shared_ptr< ISequence > >& sequencesToTrack) = 0;

        /**
         * Get the minimum sequence value from all of the gating sequences added to this ringBuffer.
         * 
         * \returns The minimum gating sequence or the cursor sequence if no sequences have been added.
         */ 
        virtual std::int64_t getMinimumSequence() = 0;

        virtual std::shared_ptr< EventPoller< T > > newPoller(const std::shared_ptr< IDataProvider< T > >& provider, const std::vector< std::shared_ptr< ISequence > >& gatingSequences) = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
