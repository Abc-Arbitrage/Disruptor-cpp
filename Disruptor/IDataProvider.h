#pragma once

#include <cstdint>
#include <memory>


namespace Disruptor
{

    template <class T>
    class IDataProvider
    {
    public:
        virtual ~IDataProvider() = default;

        virtual T& operator[](std::int64_t sequence) const = 0;
    };

} // namespace Disruptor
