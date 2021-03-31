#pragma once
#include "value.h"
#include <stack>

JSONCPP_NAMESPACE_BEGIN

class JSON_API JsonReader
{
protected:
    struct ParseState
    {
        ParseState() = default;

        explicit ParseState(JsonType JType)
            : Type(JType)
        {
            if (JType == JsonType::Object)
                Object = std::make_shared<JsonObject>();
            else
                Array = std::make_shared<JsonArray>();
        }

        JsonType                    Type = JsonType::Unknown;
        std::string                 Identifier;
        std::shared_ptr<JsonArray>  Array;
        std::shared_ptr<JsonObject> Object;
    };

                JsonReader() = default;
    virtual     ~JsonReader() = default;

    void        Parse(const char* First, const char* Last, ParseState& OutState);

public:
    virtual bool Deserialize(std::shared_ptr<JsonValue>& Root) = 0;

private:
    const char* SkipWhiteSpace(const char* First, const char* Last) const;

    const char* ParseColon(const char* First, const char* Last);
    const char* ParseComma(const char* First, const char* Last);
    const char* ParseNull(const char* First, const char* Last);
    const char* ParseBoolean(const char* First, const char* Last);
    const char* ParseDigits(const char* First, const char* Last) const;
    const char* ParseNumber(const char* First, const char* Last);
    const char* ParseUnicode(const char* First, const char* Last, uint32_t& CodePoint) const;
    const char* ParseString(const char* First, const char* Last);
    const char* ParseArray(const char* First, const char* Last, ParseState& OutState);
    const char* ParseObject(const char* First, const char* Last, ParseState& OutState);

    bool        IsProcessedType(JsonType Type) const;
    void        AppendJsonValue(std::shared_ptr<JsonValue> Value);
    void        AppendJsonIdentifier(std::string&& String);

    bool        MatchExpectedToken(JsonTokenType Token) const noexcept;

    template<class ...Args>
    void        SetExpectedToken(Args&& ...Tokens) noexcept    {   m_PrevToken = m_ExpectedToken; m_ExpectedToken = MergeTokens(Tokens...);    }
    template<class ...Args>
    void        AppendExpectedToken(Args&& ...Tokens) noexcept {   m_ExpectedToken |= MergeTokens(Tokens...);                                  }
    
private:
    std::stack<ParseState>  m_ParseProcessState;
    uint32_t                m_ExpectedToken         = 0;
    uint32_t                m_PrevToken             = 0;
};

class JSON_API JsonStringReader : public JsonReader
{
protected:
    JsonStringReader() = default;
    explicit JsonStringReader(const std::string& Content) : m_Content(Content)       {}
    explicit JsonStringReader(std::string&& Content) : m_Content(std::move(Content)) {}

public:
    virtual ~JsonStringReader() = default;

    JSON_NODISCARD static auto Create(const std::string& Content)   {   return std::unique_ptr<JsonStringReader>(new JsonStringReader(Content));                }
    JSON_NODISCARD static auto Create(std::string&& Content)        {   return std::unique_ptr<JsonStringReader>(new JsonStringReader(std::move(Content)));     }

    bool Deserialize(std::shared_ptr<JsonValue>& Root) override;

protected:
    std::string m_Content;
};

class JSON_API JsonStreamReader : public JsonStringReader
{
protected:
    explicit JsonStreamReader(std::basic_istream<char>& IStream);

public:
    virtual ~JsonStreamReader() = default;

    JSON_NODISCARD static auto Create(std::basic_istream<char>& IStream)    {   return std::unique_ptr<JsonStreamReader>(new JsonStreamReader(IStream));    }
};

class JSON_API JsonReaderFactory
{
    template<class STR>
    using EnableIfString = typename std::enable_if<std::is_convertible<STR, std::string>::value>::type;

public:
    template<class String, class = EnableIfString<String>>
    JSON_NODISCARD static auto Create(String&& Content)                    {   return JsonStringReader::Create(std::forward<String>(Content));  }
    JSON_NODISCARD static auto Create(std::basic_istream<char>& IStream)   {   return JsonStreamReader::Create(IStream);                        }

};

JSONCPP_NAMESPACE_END