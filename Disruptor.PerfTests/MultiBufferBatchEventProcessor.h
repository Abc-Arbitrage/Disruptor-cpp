#pragma once

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "Disruptor/AlertException.h"
#include "Disruptor/ArgumentException.h"
#include "Disruptor/IDataProvider.h"
#include "Disruptor/IEventHandler.h"
#include "Disruptor/IEventProcessor.h"
#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/NotSupportedException.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/TimeoutException.h"


namespace Disruptor
{
namespace PerfTests
{

    template <class T>
    class MultiBufferBatchEventProcessor : public IEventProcessor
    {
    public:
        MultiBufferBatchEventProcessor(const std::vector< std::shared_ptr< IDataProvider< T > > >& providers,
                                       const std::vector< std::shared_ptr< ISequenceBarrier > >& barriers,
                                       const std::shared_ptr< IEventHandler< T > >& handler)
        {
            if (providers.size() != barriers.size())
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("Should have as many providers as barriers (" << providers.size() << " != " << barriers.size() << ")");

            m_providers = providers;
            m_barriers = barriers;
            m_handler = handler;

            m_sequences.resize(m_providers.size());
            for (auto i = 0u; i < m_sequences.size(); ++i)
            {
                m_sequences[i] = std::make_shared< Sequence >();
            }
        }

        void run() override
        {
            if (std::atomic_exchange(&m_isRunning, 1) != 0)
                throw std::runtime_error("Already running");

            for (auto&& barrier : m_barriers)
            {
                barrier->clearAlert();
            }

            auto barrierLength = m_barriers.size();

            while (true)
            {
                try
                {
                    for (auto i = 0u; i < barrierLength; ++i)
                    {
                        auto available = m_barriers[i]->waitFor(-1);
                        auto sequence = m_sequences[i];

                        auto nextSequence = sequence->value() + 1;

                        for (auto l = nextSequence; l <= available; ++l)
                        {
                            m_handler->onEvent((*m_providers[i])[l], l, nextSequence == available);
                        }

                        sequence->setValue(available);

                        m_count += available - nextSequence + 1;
                    }

                    std::this_thread::yield();
                }
                catch (AlertException&)
                {
                    if (m_isRunning == 0)
                        break;
                }
                catch (TimeoutException& ex)
                {
                    std::cout << ex.what() << std::endl;
                }
                catch (std::exception& ex)
                {
                    std::cout << ex.what() << std::endl;
                    break;
                }
            }
        }

        std::shared_ptr< ISequence > sequence() const override
        {
            DISRUPTOR_THROW_NOT_SUPPORTED_EXCEPTION();
        }

        std::int64_t count() const
        {
            return m_count;
        }

        const std::vector< std::shared_ptr< ISequence > >& getSequences() const
        {
            return m_sequences;
        }

        void halt() override
        {
            m_isRunning = 0;
            m_barriers[0]->alert();
        }

        bool isRunning() const override
        {
            return m_isRunning == 1;
        }
        
    private:
        std::atomic< std::int32_t > m_isRunning{0};
        std::vector< std::shared_ptr< IDataProvider< T > > > m_providers;
        std::vector< std::shared_ptr< ISequenceBarrier > > m_barriers;
        std::shared_ptr< IEventHandler< T > > m_handler;
        std::vector< std::shared_ptr< ISequence > > m_sequences;
        std::int64_t m_count = 0;
    };

} // namespace PerfTests
} // namespace Disruptor
