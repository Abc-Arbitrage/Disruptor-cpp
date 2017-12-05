#include "stdafx.h"

#include "Disruptor/ArgumentException.h"
#include "Disruptor/IgnoreExceptionHandler.h"
#include "StubEvent.h"


using namespace Disruptor;


BOOST_AUTO_TEST_SUITE(IgnoreExceptionHandlerTests)

BOOST_AUTO_TEST_CASE(ShouldIgnoreException)
{
    auto causeException = ArgumentException("IgnoreExceptionHandler.ShouldIgnoreException");
    auto evt = Tests::StubEvent(0);

    auto exceptionHandler = std::make_shared< IgnoreExceptionHandler< Tests::StubEvent> >();

    BOOST_CHECK_NO_THROW(exceptionHandler->handleEventException(causeException, 0L, evt));
}

BOOST_AUTO_TEST_SUITE_END()
