#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(ArgumentOutOfRangeException);

} // namespace Disruptor


#define DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION(variable) \
    DISRUPTOR_PRAGMA_PUSH \
    DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT \
    DISRUPTOR_PRAGMA_IGNORE_UNREACHABLE_CODE \
    do \
    { \
        DISRUPTOR_THROW(::Disruptor::ArgumentOutOfRangeException, "The variable '" << #variable << "' (" << variable << ") is out of range"); \
    } while (0); \
    DISRUPTOR_PRAGMA_POP


#define DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_WITH_MESSAGE_EXCEPTION(message) DISRUPTOR_THROW(::Disruptor::ArgumentOutOfRangeException, message)
