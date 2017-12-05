#include "stdafx.h"
#include "TypeInfo.h"

#include <regex>
#include <vector>
#include <boost/algorithm/string.hpp>

#if defined(__GNUC__)
# include <cxxabi.h>
#endif /* __GNUC__ */


namespace Disruptor
{

    TypeInfo::TypeInfo(const std::type_info& typeInfo)
        : m_typeInfo(&typeInfo)
        , m_fullyQualifiedName(dotNetify(demangleTypeName(m_typeInfo->name())))
        , m_name(unqualifyName(m_fullyQualifiedName))
    {
    }

    const std::type_info& TypeInfo::intrinsicTypeInfo() const
    {
        return *m_typeInfo;
    }

    const std::string& TypeInfo::fullyQualifiedName() const
    {
        return m_fullyQualifiedName;
    }

    const std::string& TypeInfo::name() const
    {
        return m_name;
    }

    bool TypeInfo::operator==(const TypeInfo& rhs) const
    {
        return intrinsicTypeInfo() == rhs.intrinsicTypeInfo();
    }

    std::string TypeInfo::dotNetify(const std::string& typeName)
    {
        return boost::algorithm::replace_all_copy(typeName, "::", ".");
    }

    std::string TypeInfo::unqualifyName(const std::string& fullyQualifiedName)
    {
        if (fullyQualifiedName.empty())
            return std::string();

        std::vector< std::string > nameParts;
        boost::split(nameParts, fullyQualifiedName, boost::is_any_of("."));

        if (nameParts.empty())
            return std::string();

        return nameParts[nameParts.size() - 1];
    }

    std::string TypeInfo::demangleTypeName(const std::string& typeName)
    {
#if defined(__GNUC__)
            int status;

            auto demangledName = abi::__cxa_demangle(typeName.c_str(), 0, 0, &status);
            if (demangledName == nullptr)
                return typeName;

            std::string result = demangledName;
            free(demangledName);
            return result;
#else
        std::string demangled = typeName;
        demangled = std::regex_replace(demangled, std::regex("(const\\s+|\\s+const)"), std::string());
        demangled = std::regex_replace(demangled, std::regex("(volatile\\s+|\\s+volatile)"), std::string());
        demangled = std::regex_replace(demangled, std::regex("(static\\s+|\\s+static)"), std::string());
        demangled = std::regex_replace(demangled, std::regex("(class\\s+|\\s+class)"), std::string());
        demangled = std::regex_replace(demangled, std::regex("(struct\\s+|\\s+struct)"), std::string());
        return demangled;
#endif /* defined(__GNUC__) */
    }

} // namespace Disruptor
