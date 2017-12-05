#pragma once

#include <mutex>
#include <deque>


namespace Disruptor
{

    template
    <
        class T,
        class TQueue = std::deque< T >
    >
    class BlockingQueue
    {
    public:
        typedef std::mutex Mutex;
        typedef std::lock_guard< Mutex > ScopedLock;
        typedef std::unique_lock< Mutex > UniqueLock;

        void push(const T& value)
        {
            {
                ScopedLock lock(m_mutex);
                m_queue.push_back(value);
            }
            m_conditionVariable.notify_one();
        }

        void push(T&& value)
        {
            {
                ScopedLock lock(m_mutex);
                m_queue.push_back(std::move(value));
            }
            m_conditionVariable.notify_one();
        }

        bool empty() const
        {
            ScopedLock lock(m_mutex);
            return m_queue.empty();
        }

        template <class TDuration>
        bool timedWaitAndPop(T& value, const TDuration& duration)
        {
            UniqueLock lock(m_mutex);

            if (!m_conditionVariable.wait_for(lock, duration, [this]() -> bool { return !m_queue.empty(); }))
                return false;

            value = std::move(m_queue.front());
            m_queue.pop_front();
            return true;
        }
        
    private:
        TQueue m_queue;
        mutable Mutex m_mutex;
        std::condition_variable m_conditionVariable;
    };

} // namespace Disruptor
