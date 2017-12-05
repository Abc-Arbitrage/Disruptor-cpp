#pragma once

#include <gmock/gmock.h>

#include "Disruptor/ISequencer.h"
#include "Disruptor.Tests/StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class SequencerMock : public ISequencer< T >
    {
    public:
        MOCK_METHOD1(claim, void(std::int64_t sequence));

        MOCK_METHOD1(isAvailable, bool(std::int64_t sequence));
        
        MOCK_METHOD1(addGatingSequences, void(const std::vector< std::shared_ptr< ISequence > >& gatingSequences));
        MOCK_METHOD1(removeGatingSequence, bool(const std::shared_ptr< ISequence >& sequence));

        MOCK_METHOD1(newBarrier, std::shared_ptr< ISequenceBarrier >(const std::vector< std::shared_ptr< ISequence > >& sequencesToTrack));
        MOCK_METHOD0(getMinimumSequence, std::int64_t());

        MOCK_METHOD2_T(newPoller, std::shared_ptr< EventPoller< T > >(const std::shared_ptr< IDataProvider< T > >& provider, const std::vector< std::shared_ptr< ISequence > >& gatingSequences));
        
        MOCK_CONST_METHOD1(writeDescriptionTo, void(std::ostream& stream));

        MOCK_METHOD0(bufferSize, std::int32_t());

        MOCK_METHOD1(hasAvailableCapacity, bool(std::int32_t requiredCapacity));

        MOCK_METHOD0(getRemainingCapacity, std::int64_t());

        MOCK_METHOD0(next, std::int64_t());
        MOCK_METHOD1(next, std::int64_t(std::int32_t n));

        MOCK_METHOD0(tryNext, std::int64_t());
        MOCK_METHOD1(tryNext, std::int64_t(std::int32_t n));

        MOCK_METHOD1(publish, void(std::int64_t sequence));
        MOCK_METHOD2(publish, void(std::int64_t lo, std::int64_t hi));

        MOCK_CONST_METHOD0(cursor, std::int64_t());

        MOCK_METHOD2(getHighestPublishedSequence, std::int64_t(std::int64_t nextSequence, std::int64_t availableSequence));
    };

} // namespace Tests
} // namespace Disruptor