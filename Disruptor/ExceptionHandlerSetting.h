#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/ConsumerRepository.h"
#include "Disruptor/IEventHandler.h"
#include "Disruptor/IExceptionHandler.h"


namespace Disruptor
{

    /// <summary>
    /// A support class used as part of setting an exception handler for a specific event handler.
    /// For example:
    /// <code>disruptorWizard.HandleExceptionsIn(eventHandler).With(exceptionHandler);</code>
    /// </summary>
    /// <typeparam name="T">the type of event being handled.</typeparam>
    template <class T>
    class ExceptionHandlerSetting
    {
    public:
        ExceptionHandlerSetting(const std::shared_ptr< IEventHandler< T > >& eventHandler,
                                const std::shared_ptr< ConsumerRepository< T > >& consumerRepository)
            : m_eventHandler(eventHandler)
            , m_consumerRepository(consumerRepository)
        {
        }
        
        /// <summary>
        /// Specify the <see cref="IExceptionHandler{T}"/> to use with the event handler.
        /// </summary>
        /// <param name="exceptionHandler">exceptionHandler the exception handler to use.</param>
        void with(const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler)
        {
            std::dynamic_pointer_cast< BatchEventProcessor< T > >(m_consumerRepository->getEventProcessorFor(m_eventHandler))->setExceptionHandler(exceptionHandler);
            m_consumerRepository->getBarrierFor(m_eventHandler)->alert();
        }

    private:
        std::shared_ptr< IEventHandler< T > > m_eventHandler;
        std::shared_ptr< ConsumerRepository< T > > m_consumerRepository;
    };

} // namespace Disruptor
