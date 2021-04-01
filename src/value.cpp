#include "value.h"

using namespace JSONCPP_NAMESPACE;

// Json Value
const std::array<std::string, JsonValue::NumTypes> JsonValue::JsonTypeString = { "Unknown", "Null", "Boolean", "Number", "String", "Array", "Object" };

bool JsonValue::Equal(const JsonValue& Lhs, const JsonValue& Rhs) noexcept
{
    if (Lhs.GetType() != Rhs.GetType())
        return false;
    switch (Lhs.GetType())
    {
    case JsonType::Unknown:
        JSON_FALLTHROUGH;
    case JsonType::Null:
        return true;
    case JsonType::Boolean:
        return Lhs.AsBool() == Rhs.AsBool();
    case JsonType::Number:
        return Lhs.AsNumber() == Rhs.AsNumber();
    case JsonType::String:
        return Lhs.AsString() == Rhs.AsString();
    case JsonType::Array:
    {
        CReferenceArray LhsArray = Lhs.AsArray();
        CReferenceArray RhsArray = Rhs.AsArray();
        
        if (LhsArray.size() != RhsArray.size())
            return false;
        
        for (uint32_t Index = 0; Index < LhsArray.size(); ++Index)
        {
            if (*LhsArray[Index] != *RhsArray[Index])
                return false;
        }
        return true;
    }
    case JsonType::Object:
    {
        CReferenceMap LhsObject = Lhs.AsObject();
        CReferenceMap RhsObject = Rhs.AsObject();
        
        if (LhsObject.size() != RhsObject.size())
            return false;
        
        for (const auto& Iter : LhsObject)
        {
            auto Found = RhsObject.find(Iter.first);
            if (Found == RhsObject.cend())
                return false;
            if (*Iter.second != *Found->second)
                return false;
        }
        return true;
    }
    default:
        return false;
    }
}

bool JsonValue::AsBool() const
{
    bool Value;
    if (!GetBoolean(Value))
        TypeCastErrorMessage(JsonType::Boolean);
    return Value;
}

double JsonValue::AsNumber() const
{
    double Value;
    if (!GetNumber(Value))
        TypeCastErrorMessage(JsonType::Number);
    return Value;
}

JsonValue::StringType JsonValue::AsString() const
{
    StringType Value;
    if (!GetString(Value))
        TypeCastErrorMessage(JsonType::String);
    return Value;
}

JsonValue::ReferenceArray JsonValue::AsArray()
{
    PointerArray Array = nullptr;
    if (!GetArray(Array))
        TypeCastErrorMessage(JsonType::Array);
    return *Array;
}

JsonValue::CReferenceArray JsonValue::AsArray() const
{
    CPointerArray Array = nullptr;
    if (!GetArray(Array))
        TypeCastErrorMessage(JsonType::Array);
    return *Array;
}

JsonValue::ReferenceMap JsonValue::AsObject()
{
    PointerMap Object = nullptr;
    if (!GetMap(Object))
        TypeCastErrorMessage(JsonType::Object);
    return *Object;
}

JsonValue::CReferenceMap JsonValue::AsObject() const
{
    CPointerMap Object = nullptr;
    if (!GetMap(Object))
        TypeCastErrorMessage(JsonType::Object);
    return *Object;
}

void JsonValue::TypeCastErrorMessage(JsonType CastTo) const
{
    JSON_ASSERT_MESSAGE(false, 
        "Json Value of type '%s' used as a '%s'.", 
        JsonTypeString[static_cast<uint32_t>(this->GetType())].c_str(),
        JsonTypeString[static_cast<uint32_t>(CastTo)].c_str()
    );
}

// Json Boolean
JsonBoolean::JsonBoolean(ValueType Value)
    : m_Boolean(Value), JsonValue(JsonType::Boolean)
{
}

// Json Number
JsonNumber::JsonNumber(const ValueType& Value)
    : m_Number(Value), JsonValue(JsonType::Number)
{
}

JsonNumber::JsonNumber(ValueType&& Value) noexcept
    : m_Number(std::move(Value)), JsonValue(JsonType::Number)
{
}

JsonNumber& JsonNumber::operator=(const ValueType& Value)
{
    if (&m_Number != &Value)
        m_Number = Value;
    return *this;
}

JsonNumber& JsonNumber::operator=(ValueType&& Value) noexcept
{
    if (&m_Number != &Value)
        m_Number = std::move(Value);
    return *this;
}

// Json String
JsonString::JsonString(const char* Src)
    : m_String(Src), JsonValue(JsonType::String)
{
}

JsonString::JsonString(const char* Src, uint32_t Length)
    : m_String(Src, Length), JsonValue(JsonType::String)
{
}

JsonString::JsonString(const char* First, const char* Last)
    : m_String(First, Last), JsonValue(JsonType::String)
{
}

JsonString::JsonString(const ValueType& Value)
    : m_String(Value), JsonValue(JsonType::String)
{
}

JsonString::JsonString(ValueType&& Value) noexcept
    : m_String(std::move(Value)), JsonValue(JsonType::String)
{
}

JsonString& JsonString::operator=(const ValueType& Value)
{
    if (&m_String != &Value)
        m_String = Value;
    return *this;
}

JsonString& JsonString::operator=(ValueType&& Value) noexcept
{
    if (&m_String != &Value)
        m_String = std::move(Value);
    return *this;
}

char& JsonString::At(uint32_t Index)
{
    JSON_ASSERT_MESSAGE(Index < m_String.size(), "String index out of bounds.");
    return m_String[Index];
}

const char& JsonString::At(uint32_t Index) const
{
    JSON_ASSERT_MESSAGE(Index < m_String.size(), "String index out of bounds.");
    return m_String[Index];
}

// Json Array
JsonArray::JsonArray(const ContainerType& Array)
    : m_Array(Array), JsonValue(JsonType::Array)
{
}

JsonArray::JsonArray(ContainerType&& Array) noexcept
    : m_Array(std::move(Array)), JsonValue(JsonType::Array)
{
}

JsonArray& JsonArray::operator=(const ContainerType& Array)
{
    if (&m_Array != &Array)
        m_Array = Array;
    return *this;
}

JsonArray& JsonArray::operator=(ContainerType&& Array) noexcept
{
    if (&m_Array != &Array)
        m_Array = std::move(Array);
    return *this;
}

bool JsonArray::GetArray(PointerArray& OutArray)
{
    OutArray = &m_Array;
    return true;
}

bool JsonArray::GetArray(CPointerArray& OutArray) const
{
    OutArray = &m_Array;
    return true;
}

JsonArray::ValueType& JsonArray::At(uint32_t Index)
{
    JSON_ASSERT_MESSAGE(Index < m_Array.size(), "Array index out of bounds.");
    return m_Array[Index];
}

const JsonArray::ValueType& JsonArray::At(uint32_t Index) const
{
    JSON_ASSERT_MESSAGE(Index < m_Array.size(), "Array index out of bounds.");
    return m_Array[Index];
}

// Json Object
JsonObject::JsonObject(const ContainerType& Values)
    : m_Values(Values), JsonValue(JsonType::Object)
{
}

JsonObject::JsonObject(ContainerType&& Values) noexcept
    : m_Values(std::move(Values)), JsonValue(JsonType::Object)
{
}

JsonObject& JsonObject::operator=(const ContainerType& Values)
{
    if (&m_Values != &Values)
        m_Values = Values;
    return *this;
}

JsonObject& JsonObject::operator=(ContainerType&& Values) noexcept
{
    if (&m_Values != &Values)
        m_Values = std::move(Values);
    return *this;
}

JsonObject::MappedType& JsonObject::At(const KeyType& Identifier)
{
    auto Found = m_Values.find(Identifier);
    JSON_ASSERT_MESSAGE(Found != m_Values.end(), "Identifier \'%s\' - not found.", Identifier.c_str());
    return Found->second;
}

const JsonObject::MappedType& JsonObject::At(const KeyType& Identifier) const
{
    auto Found = m_Values.find(Identifier);
    JSON_ASSERT_MESSAGE(Found != m_Values.end(), "Identifier \'%s\' - not found.", Identifier.c_str());
    return Found->second;
}

bool JsonObject::Has(const KeyType& Identifier) const
{
    return m_Values.find(Identifier) != m_Values.end();
}

bool JsonObject::Insert(const KeyType& Identifier, MappedType Value)
{
    return m_Values.insert(std::make_pair(Identifier, Value)).second;
}

bool JsonObject::Insert(KeyType&& Identifier, MappedType Value)
{
    return m_Values.insert(std::make_pair(std::move(Identifier), Value)).second;
}

bool JsonObject::Emplace(KeyType&& Identifier, MappedType Value)
{
    return m_Values.emplace(std::move(Identifier), Value).second;
}

void JsonObject::Erase(const KeyType& Identifier)
{
    auto Found = m_Values.find(Identifier);
    if (Found != m_Values.end())
        m_Values.erase(Found);
}

JsonObject::MappedType JsonObject::Extract(const KeyType& Identifier)
{
    auto Found = m_Values.find(Identifier);
    if (Found != m_Values.end())
    {
        auto Value = Found->second;
        m_Values.erase(Found);
        return Value;
    }
    return nullptr;
}

JsonObject::MappedType JsonObject::Extract(ConstIterator Where)
{
    if (Where != m_Values.cend())
    {
        auto Value = Where->second;
        m_Values.erase(Where);
        return Value;
    }
    return nullptr;
}

uint32_t JsonObject::Count(JsonType Type) const noexcept
{
    uint32_t Count = 0;
    for (const auto& Value : m_Values)
    {
        if (Value.second->GetType() == Type)
            ++Count;
    }
    return Count;
}

bool JsonObject::Contains(JsonType Type) const noexcept
{
    for (const auto& Value : m_Values)
    {
        if (Value.second->GetType() == Type)
            return true;
    }
    return false;
}

bool JsonObject::GetMap(PointerMap& OutMap)
{
    OutMap = &m_Values;
    return true;
}

bool JsonObject::GetMap(CPointerMap& OutMap) const
{
    OutMap = &m_Values;
    return true;
}
