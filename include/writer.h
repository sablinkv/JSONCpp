#pragma once
#include "value.h"

JSONCPP_NAMESPACE_BEGIN

class JSON_API JsonWriter
{
protected:
	explicit JsonWriter(uint32_t Indent) : m_Indent(Indent) {}

public:
	using OStream = std::basic_ostream<char>;

	virtual ~JsonWriter() = default;
	virtual void Serialize(const JsonValue* Root) const = 0;

protected:
	void	Write(OStream& Stream, const JsonValue* Root, uint32_t Level = 1) const;

private:
	void	WriteHex(OStream& Stream, uint32_t CodePoint) const;
	void	WriteString(OStream& Stream, const std::string& String) const;
	void	WriteIndent(OStream& Stream, uint32_t Level) const;
	void	WriteObject(OStream& Stream, const JsonValue* Root, uint32_t Level) const;
	void	WriteArray(OStream& Stream, const JsonValue* Root, uint32_t Level) const;

private:
	uint32_t m_Indent;
};

class JSON_API JsonStreamWriter : public JsonWriter
{
protected:
	explicit JsonStreamWriter(OStream* Stream, uint32_t Indent) : m_Stream(Stream), JsonWriter(Indent) {}

public:
	virtual ~JsonStreamWriter() = default;

	JSON_NODISCARD static auto Create(OStream* Stream, uint32_t Indent = 2)
	{
		return std::unique_ptr<JsonStreamWriter>(new JsonStreamWriter(Stream, Indent));
	}

	void Serialize(const JsonValue* Root) const override;

private:
	OStream* m_Stream;
};

class JSON_API JsonStringWriter : public JsonWriter
{
protected:
	explicit JsonStringWriter(std::string* OutString, uint32_t Indent) : m_OutString(OutString), JsonWriter(Indent) {}

public:
	virtual ~JsonStringWriter() = default;

	JSON_NODISCARD static auto Create(std::string* OutString, uint32_t Indent = 2)
	{
		return std::unique_ptr<JsonStringWriter>(new JsonStringWriter(OutString, Indent));
	}

	void Serialize(const JsonValue* Root) const override;

private:
	std::string* m_OutString;
};

class JSON_API JsonWriterFactory
{
public:
	using OStream = std::basic_ostream<char>;
	using String = std::basic_string<char>;

	JSON_NODISCARD static auto Create(OStream* OutStream, int32_t Indent = 2)	{	return JsonStreamWriter::Create(OutStream, Indent);	}
	JSON_NODISCARD static auto Create(String* OutString, int32_t Indent = 2)	{	return JsonStringWriter::Create(OutString, Indent);	}
};

struct JSON_API Serializer
{
	bool operator()(const JsonValue& Root, JsonWriter& Writer) const;
	bool operator()(const std::shared_ptr<JsonValue>& Root, JsonWriter& Writer) const;
};

JSONCPP_NAMESPACE_END