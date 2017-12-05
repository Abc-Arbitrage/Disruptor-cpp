#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(InsufficientCapacityException);

} // namespace Disruptor


#define DISRUPTOR_THROW_INSUFFICIENT_CAPACITY_EXCEPTION() DISRUPTOR_THROW(::Disruptor::InsufficientCapacityException, "")
