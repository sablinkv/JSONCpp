#include "literal.h"
#include "utf.h"
#include "reader.h"

using namespace JSONCPP_NAMESPACE;

std::basic_string<char> Literal::operator""_utf8(const char16_t* Source, size_t Length)
{
	std::basic_string<char> u8String;
	Utf16::Transcode<char>(Source, Source + Length, std::back_inserter(u8String));
	return u8String;
}

std::basic_string<char> Literal::operator""_utf8(const char32_t* Source, size_t Length)
{
	std::basic_string<char> u8String;
	Utf32::Transcode<char>(Source, Source + Length, std::back_inserter(u8String));
	return u8String;
}

std::basic_string<char> Literal::operator""_utf8(const wchar_t* Source, size_t Length)
{
	std::basic_string<char> u8String;
	UtfW::Transcode<char>(Source, Source + Length, std::back_inserter(u8String));
	return u8String;
}

std::basic_string<char16_t> Literal::operator""_utf16(const char* Source, size_t Length)
{
	std::basic_string<char16_t> u16String;
	Utf8::Transcode<char16_t>(Source, Source + Length, std::back_inserter(u16String));
	return u16String;
}

std::basic_string<char16_t> Literal::operator""_utf16(const char32_t* Source, size_t Length)
{
	std::basic_string<char16_t> u16String;
	Utf32::Transcode<char16_t>(Source, Source + Length, std::back_inserter(u16String));
	return u16String;
}

std::basic_string<char16_t> Literal::operator""_utf16(const wchar_t* Source, size_t Length)
{
	std::basic_string<char16_t> u16String;
	UtfW::Transcode<char16_t>(Source, Source + Length, std::back_inserter(u16String));
	return u16String;
}

std::basic_string<char32_t> Literal::operator""_utf32(const char* Source, size_t Length)
{
	std::basic_string<char32_t> u32String;
	Utf8::Transcode<char32_t>(Source, Source + Length, std::back_inserter(u32String));
	return u32String;
}

std::basic_string<char32_t> Literal::operator""_utf32(const char16_t* Source, size_t Length)
{
	std::basic_string<char32_t> u32String;
	Utf16::Transcode<char32_t>(Source, Source + Length, std::back_inserter(u32String));
	return u32String;
}

std::basic_string<char32_t> Literal::operator""_utf32(const wchar_t* Source, size_t Length)
{
	std::basic_string<char32_t> u32String;
	UtfW::Transcode<char32_t>(Source, Source + Length, std::back_inserter(u32String));
	return u32String;
}

std::basic_string<wchar_t> Literal::operator""_utfw(const char* Source, size_t Length)
{
	std::basic_string<wchar_t> wString;
	Utf8::Transcode<wchar_t>(Source, Source + Length, std::back_inserter(wString));
	return wString;
}

std::basic_string<wchar_t> Literal::operator""_utfw(const char16_t* Source, size_t Length)
{
	std::basic_string<wchar_t> wString;
	Utf16::Transcode<wchar_t>(Source, Source + Length, std::back_inserter(wString));
	return wString;
}

std::basic_string<wchar_t> Literal::operator""_utfw(const char32_t* Source, size_t Length)
{
	std::basic_string<wchar_t> wString;
	Utf32::Transcode<wchar_t>(Source, Source + Length, std::back_inserter(wString));
	return wString;
}

std::shared_ptr<JsonValue> Literal::operator""_json(const char* Src, size_t Length)
{
	auto Reader = JsonReaderFactory::Create(std::string(Src, Length));
	std::shared_ptr<JsonValue> Doc;
	Reader->Parse(Doc);
	return Doc;
}