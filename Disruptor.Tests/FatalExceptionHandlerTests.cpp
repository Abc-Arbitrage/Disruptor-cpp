#include "stdafx.h"

#include "Disruptor/ArgumentException.h"
#include "Disruptor/FatalExceptionHandler.h"
#include "StubEvent.h"


using namespace Disruptor;


BOOST_AUTO_TEST_SUITE(FatalExceptionHandlerTests)

BOOST_AUTO_TEST_CASE(ShouldHandleFatalException)
{
    auto causeException = ArgumentException("FatalExceptionHandlerTests.ShouldHandleFatalException");
    auto evt = Tests::StubEvent(0);

    auto exceptionHandler = std::make_shared< FatalExceptionHandler< Tests::StubEvent> >();

    try
    {
        exceptionHandler->handleEventException(causeException, 0L, evt);
    }
    catch (FatalException& ex)
    {
        BOOST_CHECK_EQUAL(causeException.what(), ex.innerException().what());
    }
}

BOOST_AUTO_TEST_SUITE_END()