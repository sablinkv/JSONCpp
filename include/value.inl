#pragma once
#include "value.h"
#include <type_traits>

JSONCPP_NAMESPACE_BEGIN

template<class T> struct DefineJson         {   static constexpr JsonType Type = JsonType::Unknown; };
template<> struct DefineJson<JsonNull>      {   static constexpr JsonType Type = JsonType::Null;    };
template<> struct DefineJson<JsonBoolean>   {   static constexpr JsonType Type = JsonType::Boolean; };
template<> struct DefineJson<JsonNumber>    {   static constexpr JsonType Type = JsonType::Number;  };
template<> struct DefineJson<JsonString>    {   static constexpr JsonType Type = JsonType::String;  };
template<> struct DefineJson<JsonArray>     {   static constexpr JsonType Type = JsonType::Array;   };
template<> struct DefineJson<JsonObject>    {   static constexpr JsonType Type = JsonType::Object;  };

template<class T, class>
inline JsonNumber::JsonNumber(const T& Value)
    : JsonValue(JsonType::Number)
{
    SetNumber(Value);
}

template<class T>
inline JsonNumber& JsonNumber::SetNumber(const T& Value)
{
    JSON_STATIC_ASSERT(std::is_arithmetic<T>::value);
    m_Number = std::to_string(Value);
    return *this;
}

template<class T>
inline T JsonNumber::GetNumber() const
{
    JSON_STATIC_ASSERT(std::is_arithmetic<T>::value);
    std::stringstream Stream;
    Stream << m_Number;
    T Value;
    Stream >> Value;
    return Value;
}

template<class Return>
inline JsonValue::TSharedPtr<Return> JsonArray::GetValueAs(uint32_t Index) const
{
    using IsBaseOfJsonValue = std::is_base_of<JsonValue, Return>;
    JSON_STATIC_ASSERT(IsBaseOfJsonValue::value);
    auto Value = At(Index);
    if (!Value->Is<DefineJson<Return>::Type>())
        TypeCastErrorMessage(DefineJson<Return>::Type);
    return std::dynamic_pointer_cast<Return>(Value);
}

template<class Return>
inline JsonValue::TSharedPtr<Return> JsonObject::GetValueAs(const KeyType& Identifier) const
{
    using IsBaseOfJsonValue = std::is_base_of<JsonValue, Return>;
    JSON_STATIC_ASSERT(IsBaseOfJsonValue::value);
    auto Found = m_Values.find(Identifier);
    if (Found == m_Values.end())
        return nullptr;
   
    auto Value = Found->second;
    if (!Value->Is<DefineJson<Return>::Type>())
        TypeCastErrorMessage(DefineJson<Return>::Type);
    return std::dynamic_pointer_cast<Return>(Value);
}

template<JsonType Type>
inline bool JsonObject::HasType(const KeyType& Name) const
{
    auto Found = m_Values.find(Name);
    if (Found != m_Values.end())
        return Found->second->Is<Type>();
    return false;
}

JSONCPP_NAMESPACE_END
