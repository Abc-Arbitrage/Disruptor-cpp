#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(InvalidOperationException);

} // namespace Disruptor


#define DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION(message) DISRUPTOR_THROW(::Disruptor::InvalidOperationException, message)
