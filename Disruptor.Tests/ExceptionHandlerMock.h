#pragma once

#include <cstdint>
#include <memory>

#include <gmock/gmock.h>

#include "Disruptor/IExceptionHandler.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class ExceptionHandlerMock : public IExceptionHandler< T >
    {
    public:
        MOCK_METHOD3_T(handleEventException, void(const std::exception&, std::int64_t, T&));
        MOCK_METHOD1(handleOnStartException, void(const std::exception&));
        MOCK_METHOD1(handleOnShutdownException, void(const std::exception&));
        MOCK_METHOD2_T(handleOnTimeoutException, void (const std::exception&, std::int64_t));
    };

} // namespace Tests
} // namespace Disruptor
