#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(FatalException);

} // namespace Disruptor


#define DISRUPTOR_THROW_FATAL_EXCEPTION(disruptorMessage, innerException) \
    DISRUPTOR_PRAGMA_PUSH \
    DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT \
    do \
    { \
        std::stringstream disruptorStream; \
        disruptorStream << disruptorMessage; \
        throw ::Disruptor::FatalException(disruptorStream.str(), innerException, __FUNCTION__, __FILE__, __LINE__); \
    } while (0); \
    DISRUPTOR_PRAGMA_POP
