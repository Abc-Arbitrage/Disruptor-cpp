#include "stdafx.h"
#include "SequenceGroups.h"

#include "ICursored.h"
#include "ISequence.h"


namespace Disruptor
{

    void SequenceGroups::addSequences(std::shared_ptr< std::vector< std::shared_ptr< ISequence > > >& sequences,
                                      const ICursored& cursor,
                                      const std::vector< std::shared_ptr< ISequence > >& sequencesToAdd)
    {
        std::int64_t cursorSequence;
        std::shared_ptr< std::vector< std::shared_ptr< ISequence > > > updatedSequences;
        std::shared_ptr< std::vector< std::shared_ptr< ISequence > > > currentSequences;

        do
        {
            currentSequences = std::atomic_load_explicit(&sequences, std::memory_order_acquire);

            updatedSequences = std::make_shared<std::vector< std::shared_ptr< ISequence > > >(currentSequences->size() + sequencesToAdd.size());
            
            std::copy(currentSequences->begin(), currentSequences->end(), updatedSequences->begin());

            cursorSequence = cursor.cursor();

            auto index = currentSequences->size();
            for (auto&& sequence : sequencesToAdd)
            {
                sequence->setValue(cursorSequence);
                (*updatedSequences)[index++] = sequence;
            }
        } 
        while (!std::atomic_compare_exchange_weak(&sequences, &currentSequences, updatedSequences)); // TODO: explicit memory order

        cursorSequence = cursor.cursor();
        
        for (auto&& sequence : sequencesToAdd)
        {
            sequence->setValue(cursorSequence);
        }
    }

    void SequenceGroups::addSequences(std::vector< std::shared_ptr< ISequence > >& sequences, const ICursored& cursor, const std::vector< std::shared_ptr< ISequence > >& sequencesToAdd)
    {
        std::int64_t cursorSequence;

        auto updatedSize = sequences.size() + sequencesToAdd.size();

        cursorSequence = cursor.cursor();

        auto index = sequences.size();
        sequences.resize(updatedSize);

        for (auto&& sequence : sequencesToAdd)
        {
            sequence->setValue(cursorSequence);
            sequences[index++] = sequence;
        }

        cursorSequence = cursor.cursor();

        for (auto& sequence : sequencesToAdd)
        {
            sequence->setValue(cursorSequence);
        }
    }

    bool SequenceGroups::removeSequence(std::vector< std::shared_ptr< ISequence > >& sequences, const std::shared_ptr< ISequence >& sequence)
    {
        std::int32_t numToRemove = countMatching(sequences, sequence);
        if (numToRemove == 0)
            return false;

        auto oldSize = static_cast< std::int32_t >(sequences.size());

        for (auto i = 0, pos = 0; i < oldSize; ++i)
        {
            auto&& testSequence = sequences[i];
            if (sequence != testSequence)
            {
                sequences[pos++] = testSequence;
            }
        }

        return numToRemove != 0;
    }

    bool SequenceGroups::removeSequence(std::shared_ptr< std::vector< std::shared_ptr< ISequence > > >& sequences, const std::shared_ptr< ISequence >& sequence)
    {
        std::int32_t numToRemove;
        std::shared_ptr< std::vector< std::shared_ptr< ISequence > > > oldSequences;
        std::shared_ptr< std::vector< std::shared_ptr< ISequence > > > newSequences;

        do
        {
            oldSequences = std::atomic_load_explicit(&sequences, std::memory_order_acquire);

            numToRemove = countMatching(*oldSequences, sequence);

            if (numToRemove == 0)
                break;

            auto oldSize = static_cast< std::int32_t >(oldSequences->size());
            newSequences = std::make_shared< std::vector< std::shared_ptr< ISequence > > >(oldSize - numToRemove);

            for (auto i = 0, pos = 0; i < oldSize; ++i)
            {
                auto&& testSequence = (*oldSequences)[i];
                if (sequence != testSequence)
                {
                    (*newSequences)[pos++] = testSequence;
                }
            }
        } while (!std::atomic_compare_exchange_weak(&sequences, &oldSequences, newSequences));

        return numToRemove != 0;
    }

    std::int32_t SequenceGroups::countMatching(const std::vector< std::shared_ptr< ISequence > >& values, const std::shared_ptr< ISequence >& toMatch)
    {
        auto numToRemove = 0;
        for (auto&& value : values)
        {
            if (value == toMatch) // Specifically uses identity
            {
                numToRemove++;
            }
        }
        return numToRemove;
    }

} // namespace Disruptor
