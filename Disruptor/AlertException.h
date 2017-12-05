#pragma once

#include "Disruptor/ExceptionBase.h"


namespace Disruptor
{

    DISRUPTOR_DECLARE_EXCEPTION(AlertException);

} // namespace Disruptor


#define DISRUPTOR_THROW_ALERT_EXCEPTION() DISRUPTOR_THROW(::Disruptor::AlertException, "")
