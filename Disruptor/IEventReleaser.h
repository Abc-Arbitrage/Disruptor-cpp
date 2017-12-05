#pragma once


namespace Disruptor
{

    class IEventReleaser
    {
    public:
        virtual ~IEventReleaser() = default;

        virtual void release() = 0;
    };

} // namespace Disruptor
