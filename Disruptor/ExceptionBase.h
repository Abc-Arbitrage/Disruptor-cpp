#pragma once

#include <exception>
#include <sstream>
#include <type_traits>

#include "Disruptor/Pragmas.h"


#define DISRUPTOR_THROW(DisruptorExceptionType, disruptorMessage) \
    DISRUPTOR_PRAGMA_PUSH \
    DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT \
    do \
    { \
        std::stringstream disruptorStream; \
        disruptorStream << disruptorMessage; \
        throw DisruptorExceptionType(disruptorStream.str(), __FUNCTION__, __FILE__, __LINE__); \
    } while (0); \
    DISRUPTOR_PRAGMA_POP



#define DISRUPTOR_DECLARE_EXCEPTION(DisruptorExceptionType) \
class DisruptorExceptionType : public ::Disruptor::ExceptionBase< DisruptorExceptionType > \
{ \
public: \
    explicit DisruptorExceptionType(std::string message = std::string(), \
                                    std::string function = std::string(), \
                                    std::string file = std::string(), \
                                    int line = -1) \
        : ExceptionBase< DisruptorExceptionType >(std::move(message), std::move(function), std::move(file), line) \
    { \
    } \
\
    explicit DisruptorExceptionType(std::string message, \
                                    const std::exception& innerException, \
                                    std::string function = std::string(), \
                                    std::string file = std::string(), \
                                    int line = -1) \
        : ExceptionBase< DisruptorExceptionType >(std::move(message), innerException, std::move(function), std::move(file), line) \
    { \
    } \
\
    explicit DisruptorExceptionType(const std::exception& innerException, \
                                    std::string function = std::string(), \
                                    std::string file = std::string(), \
                                    int line = -1) \
        : ExceptionBase< DisruptorExceptionType >(innerException, std::move(function), std::move(file), line) \
    { \
    } \
}


namespace Disruptor
{

    template <class TException>
    class ExceptionBase : public std::exception
    {
    public:
        explicit ExceptionBase(std::string message = std::string(),
                               std::string function = std::string(),
                               std::string file = std::string(),
                               int line = -1)
            : m_message(std::move(message))
            , m_innerException(emptyException())
            , m_function(std::move(function))
            , m_file(std::move(file))
            , m_line(line)
            , m_wholeMessage()
        {
        }

        explicit ExceptionBase(const std::exception& innerException,
                               std::string function = std::string(),
                               std::string file = std::string(),
                               int line = -1)
            : m_message()
            , m_innerException(innerException)
            , m_function(std::move(function))
            , m_file(std::move(file))
            , m_line(line)
            , m_wholeMessage()
        {
        }

        explicit ExceptionBase(std::string message,
                               const std::exception& innerException,
                               std::string function = std::string(),
                               std::string file = std::string(),
                               int line = -1)
            : m_message(std::move(message))
            , m_innerException(innerException)
            , m_function(std::move(function))
            , m_file(std::move(file))
            , m_line(line)
            , m_wholeMessage()
        {
        }

        // theoricaly not necessary but g++ complains
        virtual ~ExceptionBase() throw() {}

        const char* what() const throw() override
        {
            if (m_wholeMessage.empty())
                formatWholeMessage();

            return m_wholeMessage.c_str();
        }

        const std::string& message() const { return m_message; }
        const std::string& function() const { return m_function; }
        const std::string& file() const { return m_file; }
        int line() const { return m_line; }
        const std::exception& innerException() const { return m_innerException; }

        // this stuff provides cool writing such as `throw Exception() << "Holy shit! what's wrong with id: " << id`;
        template <class T>
        TException& operator<<(const T& rhs)
        {
            std::stringstream stream;
            stream << rhs;
            m_message += stream.str();
            return static_cast< TException& >(*this);
        }

    protected:
        bool hasThrowLocationInformation() const
        {
            return m_line != -1; 
        }

        void formatWholeMessage() const
        {
            std::stringstream stream;
            
            stream << m_message;
            
            if (hasThrowLocationInformation())
            {
                stream << " - ";
                appendThrowLocationInformation(stream);
            }

            auto&& derivedMessage = getDerivedExceptionMessage();
            if (derivedMessage.size())
                stream << " - " << derivedMessage;

            m_wholeMessage = stream.str();
        }

        // let a chance for a derived exception to provide additional information, such as an api error string.
        virtual std::string getDerivedExceptionMessage() const
        {
            return std::string();
        }

        void appendThrowLocationInformation(std::stringstream& stream) const
        {
            stream << "Throw location: " << m_function << " in " << m_file << "(" << m_line << ")";
        }

        static const std::exception& emptyException()
        {
            static std::exception result;
            return result;
        }

    private:
        std::string m_message;
        const std::exception& m_innerException;
        std::string m_function;
        std::string m_file;
        int m_line;
        mutable std::string m_wholeMessage;
    };

    template <class TException>
    inline std::string toString(const ExceptionBase< TException >& ex)
    {
        std::stringstream stream;
        stream << ex;
        return stream.str();
    }

} // namespace Disruptor


#include <ostream>


namespace std
{

    template <class TException>
    inline ostream& operator<<(ostream& os, const Disruptor::ExceptionBase< TException >& ex)
    {
        return os
            << ex.message()
            << ", InnerException: " << ex.innerException().what();
    }

} // namespace std
