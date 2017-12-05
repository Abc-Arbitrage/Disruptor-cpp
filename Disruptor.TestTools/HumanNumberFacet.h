#pragma once

#include <locale>


namespace Disruptor
{
namespace Tests
{

    struct HumanNumberFacet : std::numpunct< char >
    {
        char do_thousands_sep() const override
        {
            return ' ';
        }

        std::string do_grouping() const override
        {
            return "\3";
        }
    };
    
} // namespace Tests
} // namespace Disruptor
