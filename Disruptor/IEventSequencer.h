#pragma once

#include "Disruptor/IDataProvider.h"
#include "Disruptor/ISequenced.h"


namespace Disruptor
{

    template <class T>
    class IEventSequencer : public IDataProvider< T >, public ISequenced
    {
    };

} // namespace Disruptor
