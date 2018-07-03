#pragma once

#include <ostream>
#include <type_traits>

#include "Disruptor/ArgumentException.h"
#include "Disruptor/ArgumentOutOfRangeException.h"
#include "Disruptor/BlockingWaitStrategy.h"
#include "Disruptor/ICursored.h"
#include "Disruptor/IEventSequencer.h"
#include "Disruptor/IEventTranslator.h"
#include "Disruptor/IEventTranslatorVararg.h"
#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/ISequencer.h"
#include "Disruptor/MultiProducerSequencer.h"
#include "Disruptor/ProducerType.h"
#include "Disruptor/SingleProducerSequencer.h"
#include "Disruptor/Util.h"


namespace Disruptor
{

    /**
     * Ring based store of reusable entries containing the data representing an event being exchanged between event publisher and IEventProcessors.
     * 
     * \tparam T implementation storing the data for sharing during exchange or parallel coordination of an event.
     */ 
    template <class T>
    class RingBuffer : public IEventSequencer< T >, public ICursored, public std::enable_shared_from_this< RingBuffer< T > >
    {
        static_assert(std::is_class< T >::value, "T should be a class");

        static const std::int32_t m_bufferPad = 128 / sizeof(int*);


        template <class TTranslators>
        struct DeduceTranslatorFromContainer
        {
        private:
            typedef typename std::decay< TTranslators >::type ContainerType;
            typedef typename ContainerType::value_type ValueType;

        public:
            using Type = typename ValueType::element_type;
        };

        template <class... TItem>
        static std::int32_t getGreatestLength(const std::initializer_list< TItem >&... l)
        {
            const std::size_t lengths[] = { l.size()... };
            auto length = std::numeric_limits< std::size_t >::min();
            for (auto i = 0u; i < sizeof...(l); ++i)
            {
                if (lengths[i] > length)
                    length = lengths[i];
            }

            return static_cast< std::int32_t >(length);
        }

    public:
        /**
         * Construct a RingBuffer with the full option set.
         * 
         * \param eventFactory eventFactory to create entries for filling the RingBuffer
         * \param sequencer waiting strategy employed by processorsToTrack waiting on entries becoming available.
         */ 
        RingBuffer(const std::function< T() >& eventFactory, const std::shared_ptr< ISequencer< T > >& sequencer)
        {
            m_sequencer = sequencer;
            m_bufferSize = sequencer->bufferSize();
            
            if (m_bufferSize < 1)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("bufferSize must not be less than 1");
            }

            if (Util::ceilingNextPowerOfTwo(m_bufferSize) != m_bufferSize)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("bufferSize must be a power of 2");
            }

            m_indexMask = m_bufferSize - 1;
            m_entries.resize(m_bufferSize + 2 * m_bufferPad);

            fill(eventFactory);
        }

        
        /**
         * Construct a RingBuffer with default strategies of: MultiThreadedLowContentionClaimStrategy and BlockingWaitStrategy
         * 
         * \param eventFactory eventFactory to create entries for filling the RingBuffer
         * \param bufferSize number of elements to create within the ring buffer.
         */ 
        RingBuffer(const std::function< T() >& eventFactory, std::int32_t bufferSize)
            : RingBuffer(eventFactory, std::make_shared< MultiProducerSequencer< T > >(bufferSize, std::make_shared< BlockingWaitStrategy >()))
        {
        }

        void fill(const std::function< T() >& eventFactory)
        {
            for (std::int32_t i = 0; i < m_bufferSize; ++i)
            {
                m_entries[m_bufferPad + i] = eventFactory();
            }
        }

        /**
         * Create a new multiple producer RingBuffer using the default wait strategy BlockingWaitStrategy
         *
         * \param factory used to create the events within the ring buffer.
         * \param bufferSize number of elements to create within the ring buffer.
         * \param waitStrategy used to determine how to wait for new elements to become available.
         * 
         */ 
        static std::shared_ptr< RingBuffer< T > > createMultiProducer(const std::function< T() >& factory, std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
        {
            return std::make_shared< RingBuffer< T > >(factory, std::make_shared< MultiProducerSequencer< T > >(bufferSize, waitStrategy));
        }

        /**
         * 
         * \param factory 
         * \param bufferSize 
         * 
         */ 
        static std::shared_ptr< RingBuffer< T > > createMultiProducer(const std::function< T() >& factory, std::int32_t bufferSize)
        {
            return createMultiProducer(factory, bufferSize, std::make_shared< BlockingWaitStrategy >());
        }

        /**
         * Create a new single producer RingBuffer with the specified wait strategy.
         * 
         * \param factory used to create the events within the ring buffer.
         * \param bufferSize number of elements to create within the ring buffer.
         * \param waitStrategy used to determine how to wait for new elements to become available.
         * 
         */ 
        static std::shared_ptr< RingBuffer< T > > createSingleProducer(const std::function< T() >& factory, std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
        {
            return std::make_shared< RingBuffer< T > >(factory, std::make_shared< SingleProducerSequencer< T > >(bufferSize, waitStrategy));
        }

        /**
         * Create a new single producer RingBuffer using the default wait strategy BlockingWaitStrategy
         *
         * \param factory used to create the events within the ring buffer.
         * \param bufferSize number of elements to create within the ring buffer.
         * 
         */ 
        static std::shared_ptr< RingBuffer< T > > createSingleProducer(const std::function< T() >& factory, std::int32_t bufferSize)
        {
            return createSingleProducer(factory, bufferSize, std::make_shared< BlockingWaitStrategy >());
        }

        /**
         * Create a new Ring Buffer with the specified producer type (SINGLE or MULTI)
         * 
         * \param producerType producer type to use<see cref="ProducerType"/>
         * \param factory used to create the events within the ring buffer.
         * \param bufferSize number of elements to create within the ring buffer.
         * \param waitStrategy used to determine how to wait for new elements to become available.
         * 
         */ 
        static std::shared_ptr< RingBuffer< T > > create(ProducerType producerType, const std::function< T() >& factory, std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
        {
            switch (producerType)
            {
                case ProducerType::Single:
                    return createSingleProducer(factory, bufferSize, waitStrategy);
                case ProducerType::Multi:
                    return createMultiProducer(factory, bufferSize, waitStrategy);
                default:
                    DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION(producerType);
            }
        }

        /**
         * Get the event for a given sequence in the RingBuffer.
         * 
         * \param sequence sequence for the event
         */ 
        T& operator[](std::int64_t sequence) const override
        {
            return m_entries[m_bufferPad + (static_cast< std::int32_t >(sequence) & m_indexMask)];
        }

        std::int32_t bufferSize() override
        {
            return m_bufferSize;
        }

        bool hasAvailableCapacity(std::int32_t requiredCapacity) override
        {
            return m_sequencer->hasAvailableCapacity(requiredCapacity);
        }

        std::int64_t next() override
        {
            return m_sequencer->next();
        }

        std::int64_t next(std::int32_t n) override
        {
            return m_sequencer->next(n);
        }

        std::int64_t tryNext() override
        {
            return m_sequencer->tryNext();
        }

        std::int64_t tryNext(std::int32_t n) override
        {
            return m_sequencer->tryNext(n);
        }

        /**
         * Get the current cursor value for the ring buffer.  The actual value received will depend on the type of ISequencer that is being used.
         */ 
        std::int64_t cursor() const override
        {
            return m_sequencer->cursor();
        }

        /**
         * Get the remaining capacity for this ringBuffer.
         *
         * \returns The number of slots remaining.
         */ 
        std::int64_t getRemainingCapacity() override
        {
            return m_sequencer->getRemainingCapacity();
        }

        void publish(std::int64_t sequence) override
        {
            m_sequencer->publish(sequence);
        }

        /**
         * Publish the specified sequences.  This action marks these particular messages as being available to be read.
         * 
         * \param lo the lowest sequence number to be published
         * \param hi the highest sequence number to be published
         */ 
        void publish(std::int64_t lo, std::int64_t hi) override
        {
            m_sequencer->publish(lo, hi);
        }

        //deprecated
        void resetTo(std::int64_t sequence)
        {
            m_sequencer->claim(sequence);
            m_sequencer->publish(sequence);
        }

        T& claimAndGetPreallocated(std::int64_t sequence)
        {
            m_sequencer->claim(sequence);
            return (*this)[sequence];
        }

        bool isPublished(std::int64_t sequence)
        {
            return m_sequencer->isAvailable(sequence);
        }

        void addGatingSequences(const std::vector< std::shared_ptr< ISequence > >& gatingSequences)
        {
            m_sequencer->addGatingSequences(gatingSequences);
        }

        std::int64_t getMinimumGatingSequence()
        {
            return m_sequencer->getMinimumSequence();
        }

        /**
         * Remove the specified sequence from this ringBuffer.
         * 
         * \param sequence sequence to be removed.
         * \returns true if this sequence was found, false otherwise.
         */ 
        bool removeGatingSequence(const std::shared_ptr< ISequence >& sequence)
        {
            return m_sequencer->removeGatingSequence(sequence);
        }

        /**
         * Create a new SequenceBarrier to be used by an EventProcessor to track which messages are available to be read from the ring buffer given a list of sequences to track.
         * 
         * \param sequencesToTrack the additional sequences to track
         * \returns A sequence barrier that will track the specified sequences.
         */ 
        std::shared_ptr< ISequenceBarrier > newBarrier(const std::vector< std::shared_ptr< ISequence > >& sequencesToTrack = {})
        {
            return m_sequencer->newBarrier(sequencesToTrack);
        }

        /**
         * Creates an event poller for this ring buffer gated on the supplied sequences.
         * 
         * \param gatingSequences 
         * \returns A poller that will gate on this ring buffer and the supplied sequences.
         */ 
        std::shared_ptr< EventPoller< T > > newPoller(const std::vector< std::shared_ptr< ISequence > >& gatingSequences = {})
        {
            return m_sequencer->newPoller(this->shared_from_this(), gatingSequences);
        }

        template <class TTranslator>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslator< T >, TTranslator >::value
        >
        ::type publishEvent(const std::shared_ptr< TTranslator >& translator)
        {
            auto sequence = m_sequencer->next();
            translateAndPublish(translator, sequence);
        }

        template <class TTranslator>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslator< T >, TTranslator >::value,
            bool
        >
        ::type tryPublishEvent(const std::shared_ptr< TTranslator >& translator)
        {
            try
            {
                auto sequence = m_sequencer->tryNext();
                translateAndPublish(translator, sequence);
                return true;
            }
            catch (InsufficientCapacityException&)
            {
                return false;
            }
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslatorVararg< T, TArgs... >, TTranslator >::value
        >
        ::type publishEvent(const std::shared_ptr< TTranslator >& translator, const TArgs&... args)
        {
            auto sequence = m_sequencer->next();
            translateAndPublish(translator, sequence, args...);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslatorVararg< T, TArgs... >, TTranslator >::value,
            bool
        >
        ::type tryPublishEvent(const std::shared_ptr< TTranslator >& translator, const TArgs&... args)
        {
            try
            {
                auto sequence = m_sequencer->tryNext();
                translateAndPublish(translator, sequence, args...);
                return true;
            }
            catch (InsufficientCapacityException&)
            {
                return false;
            }
        }

        template <class TTranslators, class = decltype(std::declval< TTranslators >().begin())>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslator< T >,
                typename DeduceTranslatorFromContainer< TTranslators >::Type
            >::value
        >
        ::type publishEvents(const TTranslators& translators)
        {
            publishEvents(translators, 0, static_cast< std::int32_t >(translators.size()));
        }

        template <class TTranslators, class = decltype(std::declval< TTranslators >().begin())>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslator< T >,
                typename DeduceTranslatorFromContainer< TTranslators >::Type
            >::value
        >
        ::type publishEvents(const TTranslators& translators, std::int32_t batchStartsAt, std::int32_t batchSize)
        {
            checkBounds(static_cast< std::int32_t >(translators.size()), batchStartsAt, batchSize);
            std::int64_t finalSequence = m_sequencer->next(batchSize);
            translateAndPublishBatch(translators, batchStartsAt, batchSize, finalSequence);
        }

        template <class TTranslators, class = decltype(std::declval< TTranslators >().begin())>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslator< T >,
                typename DeduceTranslatorFromContainer< TTranslators >::Type
            >::value,
            bool
        >
        ::type tryPublishEvents(const TTranslators& translators)
        {
            return tryPublishEvents(translators, 0, static_cast< std::int32_t >(translators.size()));
        }

        template <class TTranslators, class = decltype(std::declval< TTranslators >().begin())>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslator< T >,
                typename DeduceTranslatorFromContainer< TTranslators >::Type
            >::value,
            bool
        >
        ::type tryPublishEvents(const TTranslators& translators, std::int32_t batchStartsAt, std::int32_t batchSize)
        {
            checkBounds(static_cast< std::int32_t >(translators.size()), batchStartsAt, batchSize);
            try
            {
                auto finalSequence = m_sequencer->tryNext(batchSize);
                translateAndPublishBatch(translators, batchStartsAt, batchSize, finalSequence);
                return true;
            }
            catch (InsufficientCapacityException&)
            {
                return false;
            }
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslatorVararg< T, TArgs... >,
                TTranslator
            >::value
        >
        ::type publishEvents(const std::shared_ptr< TTranslator >& translator, const std::initializer_list< TArgs >&... args)
        {
            publishEvents(translator, 0, getGreatestLength(args...), args...);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslatorVararg< T, TArgs... >,
                TTranslator
            >::value
        >
        ::type publishEvents(const std::shared_ptr< TTranslator >& translator, std::int32_t batchStartsAt, std::int32_t batchSize, const std::initializer_list< TArgs >&... args)
        {
            checkBounds(getGreatestLength(args...), batchStartsAt, batchSize);
            std::int64_t finalSequence = m_sequencer->next(batchSize);
            translateAndPublishBatch(translator, batchStartsAt, batchSize, finalSequence, args...);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslatorVararg< T, TArgs... >,
                TTranslator
            >::value,
            bool
        >
        ::type tryPublishEvents(const std::shared_ptr< TTranslator >& translator, const std::initializer_list< TArgs >&... args)
        {
            return tryPublishEvents(translator, 0, getGreatestLength(args...), args...);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslatorVararg< T, TArgs... >,
                TTranslator
            >::value,
            bool
        >
        ::type tryPublishEvents(const std::shared_ptr< TTranslator >& translator, std::int32_t batchStartsAt, std::int32_t batchSize, const std::initializer_list< TArgs >&... args)
        {
            checkBounds(getGreatestLength(args...), batchStartsAt, batchSize);
            try
            {
                auto finalSequence = m_sequencer->tryNext(batchSize);
                translateAndPublishBatch(translator, batchStartsAt, batchSize, finalSequence, args...);
                return true;
            }
            catch (InsufficientCapacityException&)
            {
                return false;
            }
        }

        void writeDescriptionTo(std::ostream& stream) const
        {
            stream << "BufferSize: " << m_bufferSize << ", Sequencer: { ";
            m_sequencer->writeDescriptionTo(stream);
            stream << " }";
        }

    private:
        void checkBounds(std::int32_t argumentCount, std::int32_t batchStartsAt, std::int32_t batchSize)
        {
            checkBatchSizing(batchStartsAt, batchSize);
            batchOverRuns(argumentCount, batchStartsAt, batchSize);
        }

        void checkBatchSizing(std::int32_t batchStartsAt, std::int32_t batchSize)
        {
            if (batchStartsAt < 0 || batchSize < 0)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("Both batchStartsAt and batchSize must be positive but got: batchStartsAt " << batchStartsAt << " and batchSize " << batchSize);
            }

            if (batchSize > bufferSize())
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("The ring buffer cannot accommodate " << batchSize << " it only has space for " << bufferSize() << " entities.");
            }
        }

        static void batchOverRuns(std::int32_t argumentCount, std::int32_t batchStartsAt, std::int32_t batchSize)
        {
            if (batchStartsAt + batchSize > argumentCount)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("A batchSize of: " << batchSize <<
                    " with batchStartsAt of: " << batchStartsAt <<
                    " will overrun the available number of arguments: " << (argumentCount - batchStartsAt));
            }
        }

        template <class TTranslator>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslator< T >, TTranslator >::value
        >
        ::type translateAndPublish(const std::shared_ptr< TTranslator >& translator, std::int64_t sequence)
        {
            try
            {
                translator->translateTo((*this)[sequence], sequence);
            }
            catch (...)
            {
            }

            m_sequencer->publish(sequence);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of< IEventTranslatorVararg< T, TArgs... >, TTranslator >::value
        >
        ::type translateAndPublish(const std::shared_ptr< TTranslator >& translator, std::int64_t sequence, const TArgs&... args)
        {
            try
            {
                translator->translateTo((*this)[sequence], sequence, args...);
            }
            catch (...)
            {
            }

            m_sequencer->publish(sequence);
        }

        template <class TTranslators, class = decltype(std::declval< TTranslators >().begin())>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslator< T >,
                typename DeduceTranslatorFromContainer< TTranslators >::Type
            >::value
        >
        ::type translateAndPublishBatch(const TTranslators& translators, std::int32_t batchStartsAt, std::int32_t batchSize, std::int64_t finalSequence)
        {
            std::int64_t initialSequence = finalSequence - (batchSize - 1);
            try
            {
                auto sequence = initialSequence;
                auto batchEndsAt = batchStartsAt + batchSize;
                auto translatorIt = translators.begin() + batchStartsAt;
                for (std::int32_t i = batchStartsAt; i < batchEndsAt; ++i, ++sequence, ++translatorIt)
                {
                    auto& translator = *translatorIt;
                    translator->translateTo((*this)[sequence], sequence);
                }
            }
            catch (...)
            {
            }

            m_sequencer->publish(initialSequence, finalSequence);
        }

        template <class TTranslator, class... TArgs>
        typename std::enable_if
        <
            std::is_base_of
            <
                IEventTranslatorVararg< T, TArgs... >,
                TTranslator
            >::value
        >
        ::type translateAndPublishBatch(const std::shared_ptr< TTranslator >& translator,
                                        std::int32_t batchStartsAt,
                                        std::int32_t batchSize,
                                        std::int64_t finalSequence,
                                        const std::initializer_list< TArgs >&... args)
        {
            std::int64_t initialSequence = finalSequence - (batchSize - 1);
            try
            {
                auto sequence = initialSequence;
                auto batchEndsAt = batchStartsAt + batchSize;
                for (std::int32_t i = batchStartsAt; i < batchEndsAt; i++, sequence++)
                {
                    translator->translateTo((*this)[sequence], sequence, *(args.begin() + i)...);
                }
            }
            catch (...)
            {
            }

            m_sequencer->publish(initialSequence, finalSequence);
        }

    private:
        char padding0[56];
        mutable std::vector< T > m_entries;
        std::int32_t m_indexMask;
        std::int32_t m_bufferSize;
        std::shared_ptr< ISequencer< T > > m_sequencer;
        char padding1[40];
    };
    
} // namespace Disruptor


namespace std
{
    
    template <class T>
    ostream& operator<<(ostream& stream, const Disruptor::RingBuffer< T >& ringBuffer)
    {
        stream << "RingBuffer: { ";
        ringBuffer.writeDescriptionTo(stream);
        stream << " }";

        return stream;
    }

} // namespace std
