#pragma once

#include <cstdint>


namespace Disruptor
{
namespace PerfTests
{

    enum class Operation
    {
        Addition,
        Subtraction,
        And
    };

namespace OperationExtensions
{

    std::int64_t Op(Operation operation, std::int64_t lhs, std::int64_t rhs);

} // namespace OperationExtensions
} // namespace PerfTests
} // namespace Disruptor
