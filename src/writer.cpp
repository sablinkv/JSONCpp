#include "writer.h"
#include "utf.h"
#include <iomanip>

using namespace JSONCPP_NAMESPACE;

void JsonWriter::Write(OStream& Stream, const JsonValue* Root, uint32_t Level) const
{
	JSON_ASSERT(Root != nullptr);
	switch (Root->GetType())
	{
	case JsonType::Object:	WriteObject(Stream, Root, Level); break;
	case JsonType::Array:	WriteArray(Stream, Root, Level); break;
	case JsonType::Null:	Stream << "null"; break;
	case JsonType::Boolean: JSON_FALLTHROUGH;
	case JsonType::Number:	Stream << Root->AsString().c_str(); break;
	case JsonType::String:	WriteString(Stream, Root->AsString()); break;
	default: JSON_ASSERT_MESSAGE(true, "Invalid serialization JSON type.");
	}
}

void JsonWriter::WriteHex(OStream& Stream, uint32_t CodePoint) const
{
	Stream << "\\u" << std::hex << std::setfill('0') << std::setw(4) << (CodePoint & 0xFFFFU);
}

void JsonWriter::WriteString(OStream& Stream, const std::string& String) const
{
	auto First = String.cbegin();
	auto Last = String.cend();

	Stream << '"';
	for (; First != Last; ++First)
	{
		auto Char = *First;
		switch (Char)
		{
		case '\"': Stream << "\\\""; break;
		case '\\': Stream << "\\\\"; break;
		case '\n': Stream << "\\n"; break;
		case '\t': Stream << "\\t"; break;
		case '\b': Stream << "\\b"; break;
		case '\f': Stream << "\\f"; break;
		case '\r': Stream << "\\r"; break;
		default:
		{
			if (Char >= static_cast<char>(0x20U))
				Stream << Char;
			else
			{
				uint32_t CodePoint;
				First = Utf8::Decode(First, Last, CodePoint) - 1;
				for (; CodePoint > 0; CodePoint >>= 16)
					WriteHex(Stream, CodePoint);
			}
			break;
		}
		}
	}
	Stream << '"';
}

void JsonWriter::WriteIndent(OStream& Stream, uint32_t Level) const
{
	if (m_Indent > 0)
	{
		auto Offset = m_Indent * Level;
		Stream << std::string(Offset, ' ');
	}
}

void JsonWriter::WriteObject(OStream& Stream, const JsonValue* Root, uint32_t Level) const
{
	auto Object = Root->AsObject();

	Stream << '{';
	if (!Object.empty())
	{
		Stream << '\n';
		auto First = Object.cbegin();
		auto Last = --Object.cend();
		for (; First != Object.end(); ++First)
		{
			WriteIndent(Stream, Level);
			// Write Identifier
			WriteString(Stream, First->first);
			Stream << ':' << ' ';
			// Write Json value
			Write(Stream, First->second.get(), Level + 1);
			if (First != Last)
				Stream << ',';
			Stream << '\n';
		}
		WriteIndent(Stream, Level - 1);
	}
	Stream << '}';
}

void JsonWriter::WriteArray(OStream& Stream, const JsonValue* Root, uint32_t Level) const
{
	auto Array = Root->AsArray();

	Stream << '[';
	if (!Array.empty())
	{
		auto Length = Array.size();
		auto Last = Length - 1;
		for (uint32_t Index = 0; Index < Length; ++Index)
		{
			Write(Stream, Array[Index].get(), Level + 1);
			if (Index != Last)
				Stream << ',' << ' ';
		}
	}
	Stream << ']';
}

void JsonStreamWriter::Serialize(const JsonValue* Root) const
{
	JSON_ASSERT(m_Stream); 
	JsonWriter::Write(*m_Stream, Root);
}

void JsonStringWriter::Serialize(const JsonValue* Root) const
{
	JSON_ASSERT(m_OutString);
	std::ostringstream OStream;
	JsonWriter::Write(OStream, Root);
	*m_OutString = OStream.str();
}

bool Serializer::operator()(const JsonValue& Root, JsonWriter& Writer) const
{
	switch (Root.GetType())
	{
	case JsonType::Array: JSON_FALLTHROUGH;
	case JsonType::Object: Writer.Serialize(static_cast<const JsonValue*>(&Root));
	default: return false;
	}
	return true;
}

bool Serializer::operator()(const std::shared_ptr<JsonValue>& Root, JsonWriter& Writer) const
{
	if (Root == nullptr)
		return false;
	return this->operator()(*Root, Writer);
}