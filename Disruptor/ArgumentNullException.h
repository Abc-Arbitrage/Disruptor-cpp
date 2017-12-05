#pragma once

#include "Disruptor/ExceptionBase.h"

namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(ArgumentNullException);

} // namespace Disruptor


#define DISRUPTOR_THROW_ARGUMENT_NULL_EXCEPTION(variable) \
    DISRUPTOR_PRAGMA_PUSH \
    DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT \
    DISRUPTOR_PRAGMA_IGNORE_UNREACHABLE_CODE \
    do \
    { \
        DISRUPTOR_THROW(::Disruptor::ArgumentNullException, "The variable '" << #variable << "' is null"); \
    } while (0); \
    DISRUPTOR_PRAGMA_POP
