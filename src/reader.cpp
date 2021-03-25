#include "reader.h"
#include "utils.h"
#include "utf.h"

using namespace JSONCPP_NAMESPACE;

void JsonReader::Parse(const char* First, const char* Last, ParseState& OutState)
{
    // Json document starts with Object or Array
    SetExpectedToken(JsonTokenType::ObjectBegin, JsonTokenType::ArrayBegin);

    while (First != Last)
    {
        // Skip Whitespace
        First = SkipWhiteSpace(First, Last);

        if (First == Last) break;

        if (IsJsonNumber(*First))
        {
            First = ParseNumber(First, Last);
        }
        else
        {
            switch (*First)
            {
            case ',': First = ParseComma(First, Last); break;
            case ':': First = ParseColon(First, Last); break;
            case 'n': First = ParseNull(First, Last); break;
            case 't': JSON_FALLTHROUGH;
            case 'f': First = ParseBoolean(First, Last); break;
            case '"': First = ParseString(First, Last); break;
            case '[': JSON_FALLTHROUGH;
            case ']': First = ParseArray(First, Last, OutState); break;
            case '{': JSON_FALLTHROUGH;
            case '}': First = ParseObject(First, Last, OutState); break;
            default: JSON_ASSERT_MESSAGE(false, "Invalid json token.");
            }
        }
    }

    JSON_ASSERT(m_ParseProcessState.empty());
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::EndFile), "End of file expected.");
}

bool JsonReader::MatchExpectedToken(JsonTokenType Token) const noexcept
{
    if (Token != JsonTokenType::EndFile)
        return static_cast<bool>(m_ExpectedToken & uint16_t(Token));

    return (m_ExpectedToken == uint16_t(JsonTokenType::EndFile));
}

const char* JsonReader::SkipWhiteSpace(const char* First, const char* Last) const
{
    while (First != Last && IsWhiteSpace(*First))
        ++First;
    return First;
}

// Parse Colon token
const char* JsonReader::ParseColon(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::Colon), "Colon unexpected.");

    // Update next expected token
    SetExpectedToken(JsonTokenType::AnyValue);
    return First + 1;
}

// Parse Comma token
const char* JsonReader::ParseComma(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::Comma), "Comma unexpected.");

    // Update next expected token
    if (IsProcessedType(JsonType::Object))
        SetExpectedToken(JsonTokenType::String);
    else if (IsProcessedType(JsonType::Array))
        SetExpectedToken(JsonTokenType::AnyValue);
    else
        SetExpectedToken(JsonTokenType::EndFile);

    return First + 1;
}

// Parse Null token
const char* JsonReader::ParseNull(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::Null), "Null value unexpected.");
    JSON_ASSERT_MESSAGE(IsJsonNull(First, Last), "Invalid null value.");

    // Append Null
    AppendJsonValue(std::make_shared<JsonNull>());

    // Update next expected token
    SetExpectedToken(JsonTokenType::Comma);
    if (IsProcessedType(JsonType::Object))
        AppendExpectedToken(JsonTokenType::ObjectEnd);
    else if (IsProcessedType(JsonType::Array))
        AppendExpectedToken(JsonTokenType::ArrayEnd);
    else
        SetExpectedToken(JsonTokenType::EndFile);

    return First + 4;
}

// Parse Boolean token
const char* JsonReader::ParseBoolean(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::Boolean), "Boolean value unexpected.");
    JSON_ASSERT_MESSAGE(IsJsonBoolean(First, Last), "Invalid boolean value.");

    // Append Boolean
    bool IsTrue = (*First == 't');
    AppendJsonValue(std::make_shared<JsonBoolean>(IsTrue));

    // Update next expected token
    SetExpectedToken(JsonTokenType::Comma);
    if (IsProcessedType(JsonType::Object))
        AppendExpectedToken(JsonTokenType::ObjectEnd);
    else if (IsProcessedType(JsonType::Array))
        AppendExpectedToken(JsonTokenType::ArrayEnd);
    else
        SetExpectedToken(JsonTokenType::EndFile);

    return First + (IsTrue ? 4 : 5);
}

// Parse Number token
const char* JsonReader::ParseDigits(const char* First, const char* Last) const
{
    if (IsSign(*First)) ++First;
    while (First != Last && IsDigit(*First)) ++First;
    return First;
}

const char* JsonReader::ParseNumber(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::Number), "Number value unexpected.");

    // Integral part
    auto _Last = ParseDigits(First, Last);

    // Fractional part
    if (_Last != Last && *_Last == '.')
        _Last = ParseDigits(++_Last, Last);

    // Exponential part
    if (_Last != Last && IsExp(*_Last))
        _Last = ParseDigits(++_Last, Last);

    // Append Number
    AppendJsonValue(std::make_shared<JsonNumber>(std::string(First, _Last)));

    // Update next expected token
    SetExpectedToken(JsonTokenType::Comma);
    if (IsProcessedType(JsonType::Object))
        AppendExpectedToken(JsonTokenType::ObjectEnd);
    else if (IsProcessedType(JsonType::Array))
        AppendExpectedToken(JsonTokenType::ArrayEnd);
    else
        SetExpectedToken(JsonTokenType::EndFile);

    return _Last;
}

// Parse String token
const char* JsonReader::ParseUnicode(const char* First, const char* Last, uint32_t& CodePoint) const
{
    JSON_ASSERT_MESSAGE((Last - First > 3) , "Invalid unicode sequence.");

    CodePoint = 0;
    for (int Radix = 0; Radix < 4; ++Radix) {
        auto Char = *First++;
        CodePoint *= 16;
        JSON_ASSERT_MESSAGE(IsHex(Char), "Invalid hexadecimal digit.");
        IsDigit(Char)
            ? CodePoint += Char - '0' 
            : CodePoint += std::toupper(Char) - 'A' + 10;
    }
    return First;
}

const char* JsonReader::ParseString(const char* First, const char* Last)
{
    JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::String), "String value unexpected.");

    auto _First = ++First;  // Skip character " - string begin
    std::string String;

    // Find end String character
    while (_First != Last)
    {
        if (*_First == '\"')
            break;

        if (*_First == '\\')
        {
            ++_First;
            if (_First == Last) break;
            switch (*_First)
            {
            case '\"': JSON_FALLTHROUGH;
            case '\\': JSON_FALLTHROUGH;
            case '/': String += *_First; break;
            case 'f': String += '\f'; break;
            case 'r': String += '\r'; break;
            case 'n': String += '\n'; break;
            case 'b': String += '\b'; break;
            case 't': String += '\t'; break;
            case 'u':
            {
                uint32_t CodePoint;
                _First = ParseUnicode(++_First, Last, CodePoint) - 1;
                String += Utf8::Encode(CodePoint);
                break;
            }
            default:
                JSON_ASSERT_MESSAGE(false, "Invalid escape character in string.");
            }
        }
        else
        {
            String += *_First;
        }
        ++_First;
    }

    JSON_ASSERT_MESSAGE(_First != Last, "Unexpected end of string.");

    // Append String or Identifier
    if (m_PrevToken & static_cast<uint32_t>(JsonTokenType::Colon))
        AppendJsonValue(std::make_shared<JsonString>(std::move(String)));
    else
        AppendJsonIdentifier(std::move(String));

    // Update next expected token
    if (IsProcessedType(JsonType::Object))
    {
        if (m_PrevToken & static_cast<uint32_t>(JsonTokenType::Colon))
            SetExpectedToken(JsonTokenType::Comma, JsonTokenType::ObjectEnd);
        else
            SetExpectedToken(JsonTokenType::Colon);
    }
    else if (IsProcessedType(JsonType::Array))
        SetExpectedToken(JsonTokenType::Comma, JsonTokenType::ArrayEnd);
    else
        SetExpectedToken(JsonTokenType::EndFile);

    return ++_First; // Skip character " - string end
}

// Parse Array token
const char* JsonReader::ParseArray(const char* First, const char* Last, ParseState& OutState)
{
    auto _Last = First + 1;
    if (*First == '[')
    {
        JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::ArrayBegin), "Array begin unexpected.");

        // Open Array
        m_ParseProcessState.emplace(ParseState(JsonType::Array));

        // Update next expected token
        SetExpectedToken(JsonTokenType::AnyValue, JsonTokenType::ArrayEnd);
    }
    else // *First == CharType(']')
    {
        JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::ArrayEnd), "Array end unexpected.");

        // Close Array
        auto State = m_ParseProcessState.top();
        m_ParseProcessState.pop();
        if (m_ParseProcessState.empty())
            OutState = State;
        else
            AppendJsonValue(State.Array);

        // Update next expected token
        SetExpectedToken(JsonTokenType::Comma);
        if (IsProcessedType(JsonType::Object))
            AppendExpectedToken(JsonTokenType::ObjectEnd);
        else if (IsProcessedType(JsonType::Array))
            AppendExpectedToken(JsonTokenType::ArrayEnd);
        else
            SetExpectedToken(JsonTokenType::EndFile);
    }

    return _Last;
}

// Parse Object token
const char* JsonReader::ParseObject(const char* First, const char* Last, ParseState& OutState)
{
    auto _Last = First + 1;
    if (*First == '{')
    {
        JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::ObjectBegin), "Object begin unexpected.");

        // Open Object
        m_ParseProcessState.emplace(ParseState(JsonType::Object));

        // Update next expected token
        SetExpectedToken(JsonTokenType::String, JsonTokenType::ObjectEnd);
    }
    else // *First == CharType('}')
    {
        JSON_ASSERT_MESSAGE(MatchExpectedToken(JsonTokenType::ObjectEnd), "Object end unexpected.");

        // Close Object
        auto State = m_ParseProcessState.top();
        m_ParseProcessState.pop();
        if (m_ParseProcessState.empty())
            OutState = State;
        else
            AppendJsonValue(State.Object);

        // Update next expected token
        SetExpectedToken(JsonTokenType::Comma);
        if (IsProcessedType(JsonType::Object))
            AppendExpectedToken(JsonTokenType::ObjectEnd);
        else if (IsProcessedType(JsonType::Array))
            AppendExpectedToken(JsonTokenType::ArrayEnd);
        else
            SetExpectedToken(JsonTokenType::EndFile);
    }

    return _Last;
}

bool JsonReader::IsProcessedType(JsonType Type) const
{
    return m_ParseProcessState.empty() ? false : m_ParseProcessState.top().Type == Type;
}

void JsonReader::AppendJsonValue(std::shared_ptr<JsonValue> Value)
{
    JSON_ASSERT(!m_ParseProcessState.empty());
    ParseState& Current = m_ParseProcessState.top();
    if (Current.Type == JsonType::Object)
    {
        auto Found = Current.Object->Find(Current.Identifier);
        JSON_ASSERT_MESSAGE(
            Found == Current.Object->CEnd(),
            "A member with the name '%s' already exists.", 
            Current.Identifier.c_str()
        );
        Current.Object->Emplace(std::move(Current.Identifier), std::move(Value));
    }
    else
    {
        Current.Array->PushBack(std::move(Value));
    }
}

void JsonReader::AppendJsonIdentifier(std::string&& String)
{
    JSON_ASSERT(!m_ParseProcessState.empty());
    ParseState& Current = m_ParseProcessState.top();
    Current.Identifier = std::move(String);
}

bool JsonStringReader::Parse(std::shared_ptr<JsonValue>& Root)
{
    ParseState OutState;
    JsonReader::Parse(m_Content.c_str(), m_Content.c_str() + m_Content.size(), OutState);

    switch (OutState.Type)
    {
    case JsonType::Object:
    {
        if (OutState.Object == nullptr) return false;
        Root = std::move(OutState.Object);
        break;
    }
    case JsonType::Array:
        if (OutState.Array == nullptr) return false;
        Root = std::move(OutState.Array);
        break;
    default:
        return false;
    }
    return true;
}

JsonStreamReader::JsonStreamReader(std::basic_istream<char>& OStream)
{
    std::ostringstream Doc;
    Doc << OStream.rdbuf();
    m_Content = std::move(Doc.str());
}
