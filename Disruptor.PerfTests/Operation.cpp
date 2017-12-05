#include "stdafx.h"
#include "Operation.h"

#include "Disruptor/ArgumentOutOfRangeException.h"


namespace Disruptor
{
namespace PerfTests
{
namespace OperationExtensions
{

    std::int64_t Op(Operation operation, std::int64_t lhs, std::int64_t rhs)
    {
        switch (operation)
        {
        case Operation::Addition:
            return lhs + rhs;
        case Operation::Subtraction:
            return lhs - rhs;
        case Operation::And:
            return lhs & rhs;
        default:
            DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION("The variable 'operation' (" << static_cast< int >(operation) << ") is out of range");
        }
    }

} // namespace OperationExtensions
} // namespace PerfTests
} // namespace Disruptor
