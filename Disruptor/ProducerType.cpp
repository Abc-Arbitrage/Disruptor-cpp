#include "stdafx.h"
#include "ProducerType.h"


namespace std
{
    
    ostream& operator<<(ostream& stream, const Disruptor::ProducerType& value)
    {
        switch (value)
        {
        case Disruptor::ProducerType::Single:
            return stream << "Single";
        case Disruptor::ProducerType::Multi:
            return stream << "Multi";
        default:
            return stream << static_cast< int >(value);
        }
    }

} // namespace std
