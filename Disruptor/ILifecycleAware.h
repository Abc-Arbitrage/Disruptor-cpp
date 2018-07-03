#pragma once


namespace Disruptor
{

    /**
     * Implement this interface in your IEventHandler<T> to be notified when a thread for the BatchEventProcessor<T> starts and shuts down.
     */ 
    class ILifecycleAware
    {
    public:
        virtual ~ILifecycleAware() = default;

        /**
         * Called once on thread start before first event is available.
         */ 
        virtual void onStart() = 0;

        /**
         * Called once just before the thread is shutdown. Sequence event processing will already have stopped before this method is called. No events wil be processed after this message.
         */ 
        virtual void onShutdown() = 0;
    };

} // namespace Disruptor
