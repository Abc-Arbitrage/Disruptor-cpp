#pragma once

#include <cstdint>
#include <memory>

#include <gmock/gmock.h>

#include "Disruptor/IDataProvider.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class DataProviderMock : public IDataProvider< T >
    {
    public:
        MOCK_CONST_METHOD1_T(indexer, T&(std::int64_t sequence));

        T& operator[](std::int64_t sequence) const override
        {
            return this->indexer(sequence);
        }
    };

} // namespace Tests
} // namespace Disruptor
