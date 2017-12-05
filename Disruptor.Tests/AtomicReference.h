#pragma once

#include <boost/optional.hpp>


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class AtomicReference
    {
    public:
        explicit AtomicReference(const boost::optional< T >& value = boost::none)
            : m_value(value)
        {
        }

        boost::optional< T > read()
        {
            return m_value;
        }

        void write(const boost::optional< T >& value)
        {
            m_value = value;
        }

    private:
        boost::optional< T > m_value;
    };

} // namespace Tests
} // namespace Disruptor
