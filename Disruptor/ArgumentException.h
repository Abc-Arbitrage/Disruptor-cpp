#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(ArgumentException);

} // namespace Disruptor


#define DISRUPTOR_THROW_ARGUMENT_EXCEPTION(message) DISRUPTOR_THROW(::Disruptor::ArgumentException, message)
