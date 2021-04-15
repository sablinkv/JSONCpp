#pragma once
#include "error.h"
#include "type.h"
#include <memory>
#include <sstream>
#include <array>
#include <vector>
#include <map>

JSONCPP_NAMESPACE_BEGIN

// Class JsonValue
class JSON_API JsonValue
{
protected:
    template<class T>
    using TSharedPtr            = std::shared_ptr<T>;
    using BooleanType           = bool;
    using NumberType            = std::basic_string<char>;
    using StringType            = std::basic_string<char>;
    using ArrayContainerType    = std::vector<TSharedPtr<JsonValue>>;
    using ObjectContainerType   = std::map<StringType, TSharedPtr<JsonValue>>;

    using PointerArray          = ArrayContainerType*;
    using CPointerArray         = const ArrayContainerType*;
    using ReferenceArray        = ArrayContainerType&;
    using CReferenceArray       = const ArrayContainerType&;

    using PointerMap            = ObjectContainerType*;
    using CPointerMap           = const ObjectContainerType*;
    using ReferenceMap          = ObjectContainerType&;
    using CReferenceMap         = const ObjectContainerType&;

    explicit JsonValue(JsonType Type = JsonType::Unknown) : m_Type(Type) {}
    virtual ~JsonValue() = default;

public:
    static constexpr uint32_t NumTypes = static_cast<uint32_t>(JsonType::Object) + 1;
    static const std::array<std::string, NumTypes> JsonTypeString;

    template<JsonType Type>
    bool                Is() const noexcept                             {   return m_Type == Type;      }
    JsonType            GetType() const noexcept                        {   return m_Type;              }

    virtual bool        GetBoolean(bool& OutBool) const                 {   return false;               }
    virtual bool        GetNumber(double& OutNumber) const              {   return false;               }
    virtual bool        GetString(StringType& OutString) const          {   return false;               }
    virtual bool        GetArray(PointerArray& OutArray)                {   return false;               }
    virtual bool        GetArray(CPointerArray& OutArray) const         {   return false;               }
    virtual bool        GetMap(PointerMap& OutMap)                      {   return false;               }
    virtual bool        GetMap(CPointerMap& OutMap) const               {   return false;               }

    static bool         Equal(const JsonValue& Lhs, const JsonValue& Rhs) noexcept;
    bool                operator==(const JsonValue& Rhs) const noexcept {   return Equal(*this, Rhs);   }
    bool                operator!=(const JsonValue& Rhs) const noexcept {   return !Equal(*this, Rhs);  }

    bool                AsBool() const;
    double              AsNumber() const;
    StringType          AsString() const;
    ReferenceArray      AsArray();
    CReferenceArray     AsArray() const;
    ReferenceMap        AsObject();
    CReferenceMap       AsObject() const;

protected:
    void                TypeCastErrorMessage(JsonType CastTo) const;

protected:
    JsonType m_Type;
};

// Class JsonNull
class JSON_API JsonNull : public JsonValue
{
public:
    JsonNull() : JsonValue(JsonType::Null) {}

    // Comparison
    bool    operator==(const JsonNull& Rhs) const noexcept  {   return true;    }
    bool    operator!=(const JsonNull& Rhs) const noexcept  {   return false;   }
};

// Class JsonBoolean
class JSON_API JsonBoolean : public JsonValue
{
public:
    using ValueType = BooleanType;

    explicit        JsonBoolean(ValueType Value = false);
    
    bool            IsTrue() const noexcept                             {   return m_Boolean == true;                               }
    bool            IsFalse() const noexcept                            {   return !IsTrue();                                       }

    JsonBoolean&    SetBoolean(ValueType Value) noexcept                {   m_Boolean = Value; return *this;                        }

    bool            GetBoolean() const noexcept                         {   return m_Boolean;                                       }
    bool            GetBoolean(bool& OutBool) const override            {   OutBool = m_Boolean; return true;                       }
    bool            GetNumber(double& OutNumber) const override         {   OutNumber = m_Boolean; return true;                     }
    bool            GetString(StringType& OutString) const override     {   OutString = IsTrue() ? "true" : "false"; return true;   }

    // Comparison
    bool            operator==(const JsonBoolean& Rhs) const noexcept   {   return m_Boolean == Rhs.m_Boolean;                      }
    bool            operator!=(const JsonBoolean& Rhs) const noexcept   {   return !(*this == Rhs);                                 }

private:
    ValueType m_Boolean;
};

// Class JsonNumber
class JSON_API JsonNumber : public JsonValue
{
public:
    using ValueType = NumberType;

    template<class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    explicit    JsonNumber(const T& Value = T(0));
    explicit    JsonNumber(const ValueType& Value);
    explicit    JsonNumber(ValueType&& Value) noexcept;

    JsonNumber& operator=(double Value)                             {   return SetNumber(Value);                        }
    JsonNumber& operator=(const ValueType& Value);
    JsonNumber& operator=(ValueType&& Value) noexcept;

    template<class T>
    JsonNumber& SetNumber(const T& Value);

    template<class T>
    T           GetNumber() const;

    template<class T>
    bool        GetNumber(T& OutNumber) const                       {   OutNumber = GetNumber<T>(); return true;        }

    bool        GetBoolean(bool& OutBool) const override            {   OutBool = GetNumber<bool>(); return true;       }
    bool        GetNumber(double& OutNumber) const override         {   OutNumber = GetNumber<double>(); return true;   }
    bool        GetString(StringType& OutString) const override     {   OutString = m_Number; return true;              }

    // Comparison
    bool        operator==(const JsonNumber& Rhs) const noexcept;
    bool        operator!=(const JsonNumber& Rhs) const noexcept    {   return !(*this == Rhs);                         }

    // Cast to aithmetical types
    template<class T>
    operator T() const                                              {   return GetNumber<T>();                          }

private:
    ValueType   m_Number;
};

// Class JsonString
class JSON_API JsonString : public JsonValue
{
public:
    using ValueType             = StringType;
    using SizeType              = typename ValueType::size_type;
    using Pointer               = typename ValueType::pointer;
    using ConstPointer          = typename ValueType::const_pointer;
    using Reference             = typename ValueType::reference;
    using ConstReference        = typename ValueType::const_reference;
    
    using Iterator              = typename ValueType::iterator;
    using ConstIterator         = typename ValueType::const_iterator;
    using ReverseIterator       = typename ValueType::reverse_iterator;
    using ConstReverseIterator  = typename ValueType::const_reverse_iterator;

                            JsonString(const char* Src, uint32_t Length);
                            JsonString(const char* First, const char* Last);
    explicit                JsonString(const char* Src);
    explicit                JsonString(const ValueType& Value);
    explicit                JsonString(ValueType&& Value) noexcept;

    JsonString&             operator=(const ValueType& Value);
    JsonString&             operator=(ValueType&& Value) noexcept;

    StringType              GetString() const noexcept                                      {   return m_String;                                            }
    bool                    GetString(ValueType& OutString) const override                  {   OutString = m_String; return true;                          }
    
    // Element access
    Reference               At(uint32_t Index);
    ConstReference          At(uint32_t Index) const;
    ConstPointer            Data() const noexcept                                           {   return m_String.data();                                     }
    ConstPointer            C_Str() const noexcept                                          {   return m_String.c_str();                                    }

    Reference               operator[](uint32_t Index) noexcept                             {   return m_String[Index];                                     }
    ConstReference          operator[](uint32_t Index) const noexcept                       {   return m_String[Index];                                     }

    // Iterator
    Iterator                Begin() noexcept                                                {   return m_String.begin();                                    }
    Iterator                End() noexcept                                                  {   return m_String.end();                                      }
    ConstIterator           CBegin() const noexcept                                         {   return m_String.cbegin();                                   }
    ConstIterator           CEnd() const noexcept                                           {   return m_String.cend();                                     }
    ReverseIterator         RBegin() noexcept                                               {   return m_String.rbegin();                                   }
    ReverseIterator         REnd() noexcept                                                 {   return m_String.rend();                                     }
    ConstReverseIterator    CRbegin() const noexcept                                        {   return m_String.crbegin();                                  }
    ConstReverseIterator    CREnd() const noexcept                                          {   return m_String.crend();                                    }

    // Capacity
    bool                    Empty() const noexcept                                          {   return m_String.empty();                                    }
    SizeType                Size() const noexcept                                           {   return m_String.size();                                     }
    SizeType                Length() const noexcept                                         {   return m_String.length();                                   }
    SizeType                Capacity() const noexcept                                       {   return m_String.capacity();                                 }
    SizeType                MaxSize() const noexcept                                        {   return m_String.max_size();                                 }
    void                    Reserve(uint32_t Count)                                         {   m_String.reserve(Count);                                    }

    // Modifiers
    void                    Clear() noexcept                                                {   m_String.clear();                                           }
    void                    PopBack() noexcept                                              {   m_String.pop_back();                                        }
    void                    PushBack(const char Char)                                       {   m_String.push_back(Char);                                   }
    void                    Resize(uint32_t Count)                                          {   m_String.resize(Count);                                     }
    void                    Resize(uint32_t Count, char Char)                               {   m_String.resize(Count, Char);                               }

    template<class FwdIter>
    JsonString&             Append(FwdIter First, FwdIter Last)                             {   m_String.append(First, Last); return *this;                 }
    JsonString&             Append(uint32_t Count, const char Char)                         {   m_String.append(Count, Char); return *this;                 }
    JsonString&             Append(const JsonString& Other)                                 {   m_String.append(Other.m_String); return *this;              }
    JsonString&             Append(const JsonString& Other, uint32_t Pos, uint32_t Count)   {   m_String.append(Other.m_String, Pos, Count);  return *this; }
    JsonString&             Append(const char* Src, uint32_t Count)                         {   m_String.append(Src, Count); return *this;                  }
    JsonString&             Append(const char* Src)                                         {   m_String.append(Src); return *this;                         }

    // Comparison
    bool                    operator==(const JsonString& Rhs) const noexcept                {   return m_String == Rhs.m_String;                            }
    bool                    operator!=(const JsonString& Rhs) const noexcept                {   return !(*this == Rhs);                                     }

private:
    ValueType m_String;
};

// Class JsonArray
class JSON_API JsonArray : public JsonValue
{
public:
    using ContainerType         = ArrayContainerType;

    using ValueType             = typename ContainerType::value_type;
    using SizeType              = typename ContainerType::size_type;
    using Pointer               = typename ContainerType::pointer;
    using ConstPointer          = typename ContainerType::const_pointer;  
    using Reference             = typename ContainerType::reference;
    using ConstReference        = typename ContainerType::const_reference;
    
    using Iterator              = typename ContainerType::iterator;
    using ConstIterator         = typename ContainerType::const_iterator;
    using ReverseIterator       = typename ContainerType::reverse_iterator;
    using ConstReverseIterator  = typename ContainerType::const_reverse_iterator;


                            JsonArray() : JsonValue(JsonType::Array)                {}
    explicit                JsonArray(const ContainerType& Array);
    explicit                JsonArray(ContainerType&& Array) noexcept;

    JsonArray&              operator=(const ContainerType& Array);
    JsonArray&              operator=(ContainerType&& Array) noexcept;
    
    // Element access
    template<class Return = JsonValue>
    TSharedPtr<Return>      GetValueAs(uint32_t Index) const;

    ValueType&              At(uint32_t Index);
    const ValueType&        At(uint32_t Index) const;

    Reference               Front() noexcept                                        {   return m_Array.front();         }
    ConstReference          Front() const noexcept                                  {   return m_Array.front();         }
    Reference               Back() noexcept                                         {   return m_Array.back();          }
    ConstReference          Back() const noexcept                                   {   return m_Array.back();          }
    Pointer                 Data() noexcept                                         {   return m_Array.data();          }
    ConstPointer            Data() const noexcept                                   {   return m_Array.data();          }

    Reference               operator[](uint32_t Index) noexcept                     {   return m_Array[Index];          }
    ConstReference          operator[](uint32_t Index) const noexcept               {   return m_Array[Index];          }

    // Iterator
    Iterator                Begin() noexcept                                        {   return m_Array.begin();         }
    Iterator                End() noexcept                                          {   return m_Array.end();           }
    ConstIterator           CBegin() const noexcept                                 {   return m_Array.cbegin();        }
    ConstIterator           CEnd() const noexcept                                   {   return m_Array.cend();          }
    ReverseIterator         RBegin() noexcept                                       {   return m_Array.rbegin();        }
    ReverseIterator         REnd() noexcept                                         {   return m_Array.rend();          }
    ConstReverseIterator    CRbegin() const noexcept                                {   return m_Array.crbegin();       }
    ConstReverseIterator    CREnd() const noexcept                                  {   return m_Array.crend();         }

    // Lookup
    template<JsonType Type>
    bool                    HasType(uint32_t Index) const                           {   return At(Index)->Is<Type>();   }

    // Capacity             
    bool                    Empty() const noexcept                                  {   return m_Array.empty();         }
    SizeType                Size() const noexcept                                   {   return m_Array.size();          }
    SizeType                Capacity() const noexcept                               {   return m_Array.capacity();      }
    void                    Reserve(uint32_t Size)                                  {   m_Array.reserve(Size);          }

    // Modifiers
    void                    Clear() noexcept                                        {   m_Array.clear();                }
    void                    PopBack() noexcept                                      {   m_Array.pop_back();             }
    void                    PushBack(ValueType Value)                               {   m_Array.push_back(Value);       }
    void                    Erase(ConstIterator Where) noexcept                     {   m_Array.erase(Where);           }
    void                    Insert(ConstIterator Where, ValueType Value)            {   m_Array.insert(Where, Value);   }
    void                    Resize(uint32_t Count, ValueType Value = ValueType())   {   m_Array.resize(Count, Value);   }

    bool                    GetArray(PointerArray& OutArray) override;
    bool                    GetArray(CPointerArray& OutArray) const override;

    // Comparison
    bool                    operator==(const JsonArray& Rhs) const noexcept         {   return m_Array == Rhs.m_Array;  }
    bool                    operator!=(const JsonArray& Rhs) const noexcept         {   return !(*this == Rhs);         }
    
private:
    ContainerType m_Array;
};

// Class JsonObject
class JSON_API JsonObject : public JsonValue
{
public:
    using ContainerType         = ObjectContainerType;

    using KeyType               = typename ContainerType::key_type;
    using MappedType            = typename ContainerType::mapped_type;
    using ValueType             = typename ContainerType::value_type;
    using SizeType              = typename ContainerType::size_type;

    using Iterator              = typename ContainerType::iterator;
    using ConstIterator         = typename ContainerType::const_iterator;
    using ReverseIterator       = typename ContainerType::reverse_iterator;
    using ConstReverseIterator  = typename ContainerType::const_reverse_iterator;

                            JsonObject() : JsonValue(JsonType::Object)  {}
    explicit                JsonObject(const ContainerType& Values);
    explicit                JsonObject(ContainerType&& Values) noexcept;

    JsonObject&             operator=(const ContainerType& Values);
    JsonObject&             operator=(ContainerType&& Values) noexcept;

    // Element access
    template<class Return = JsonValue>
    TSharedPtr<Return>      GetValueAs(const KeyType& Identifier) const;

    MappedType&             At(const KeyType& Identifier);
    const MappedType&       At(const KeyType& Identifier) const;

    MappedType&             operator[](const KeyType& Identifier)               {   return m_Values[Identifier];                        }
    MappedType&             operator[](KeyType&& Identifier)                    {   return m_Values[std::move(Identifier)];             }

    // Iterator
    Iterator                Begin() noexcept                                    {   return m_Values.begin();                            }
    Iterator                End() noexcept                                      {   return m_Values.end();                              }
    ConstIterator           CBegin() const noexcept                             {   return m_Values.cbegin();                           }
    ConstIterator           CEnd() const noexcept                               {   return m_Values.cend();                             }
    ReverseIterator         RBegin() noexcept                                   {   return m_Values.rbegin();                           }
    ReverseIterator         REnd() noexcept                                     {   return m_Values.rend();                             }
    ConstReverseIterator    CRBegin() const noexcept                            {   return m_Values.crbegin();                          }
    ConstReverseIterator    CREnd() const noexcept                              {   return m_Values.crend();                            }

    // Lookup
    template<JsonType Type>
    bool                HasType(const KeyType& Identifier) const;
    bool                Has(const KeyType& Identifier) const;
    Iterator            Find(const KeyType& Identifier)                     {   return m_Values.find(Identifier);                   }
    ConstIterator       Find(const KeyType& Identifier) const               {   return m_Values.find(Identifier);                   }
    uint32_t            Count(JsonType Type) const noexcept;
    bool                Contains(JsonType Type) const noexcept;

    // Capacity
    bool                Empty() const noexcept                              {   return m_Values.empty();                            }
    SizeType            Size() const noexcept                               {   return m_Values.size();                             }

    // Modifiers
    void                Clear() noexcept                                    {   m_Values.clear();                                   }
    void                Erase(const KeyType& Identifier);
    void                Erase(ConstIterator Where)                          {   m_Values.erase(Where);                              }
    bool                Insert(const ValueType& Value)                      {   return m_Values.insert(Value).second;               }
    bool                Insert(ValueType&& Value)                           {   return m_Values.insert(std::move(Value)).second;    }
    bool                Insert(const KeyType& Identifier, MappedType Value);
    bool                Insert(KeyType&& Identifier, MappedType Value);
    MappedType          Extract(ConstIterator Where);
    MappedType          Extract(const KeyType& Identifier);
    bool                Emplace(KeyType&& Identifier, MappedType Value);

    bool                GetMap(PointerMap& OutMap) override;
    bool                GetMap(CPointerMap& OutMap) const override;

    // Comparison
    bool                operator==(const JsonObject& Rhs) const noexcept    {   return m_Values == Rhs.m_Values;                    }
    bool                operator!=(const JsonObject& Rhs) const noexcept    {   return !(*this == Rhs);                             }

private:
    ContainerType m_Values;
};

JSONCPP_NAMESPACE_END

#include "value.inl"