#pragma once


namespace Disruptor
{

    class ITimeoutHandler
    {
    public:
        virtual ~ITimeoutHandler() = default;

        virtual void onTimeout(std::int64_t sequence) = 0;
    };

} // namespace Disruptor
