#pragma once

#include <cstdint>


namespace Disruptor
{

    class ICursored;
    class ISequence;

    /**
     * Provides static methods for managing a SequenceGroup object
     */
    class SequenceGroups
    {
    public:
        static void addSequences(std::shared_ptr< std::vector< std::shared_ptr< ISequence > > >& sequences,
                                 const ICursored& cursor,
                                 const std::vector< std::shared_ptr< ISequence > >& sequencesToAdd);

        static void addSequences(std::vector< std::shared_ptr< ISequence > >& sequences,
                                 const ICursored& cursor,
                                 const std::vector< std::shared_ptr< ISequence > >& sequencesToAdd);

        static bool removeSequence(std::shared_ptr< std::vector< std::shared_ptr< ISequence > > >& sequences, const std::shared_ptr< ISequence >& sequence);
        static bool removeSequence(std::vector< std::shared_ptr< ISequence > >& sequences, const std::shared_ptr< ISequence >& sequence);

    private:
        static std::int32_t countMatching(const std::vector< std::shared_ptr< ISequence > >& values, const std::shared_ptr< ISequence >& toMatch);
    };

} // namespace Disruptor
