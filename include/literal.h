#pragma once
#include "value.h"

JSONCPP_NAMESPACE_BEGIN

namespace Literal {
	std::basic_string<char> operator"" _utf8(const char16_t* Source, size_t Length);
	std::basic_string<char> operator"" _utf8(const char32_t* Source, size_t Length);
	std::basic_string<char> operator"" _utf8(const wchar_t* Source, size_t Length);

	std::basic_string<char16_t> operator"" _utf16(const char* Source, size_t Length);
	std::basic_string<char16_t> operator"" _utf16(const char32_t* Source, size_t Length);
	std::basic_string<char16_t> operator"" _utf16(const wchar_t* Source, size_t Length);

	std::basic_string<char32_t> operator"" _utf32(const char* Source, size_t Length);
	std::basic_string<char32_t> operator"" _utf32(const char16_t* Source, size_t Length);
	std::basic_string<char32_t> operator"" _utf32(const wchar_t* Source, size_t Length);

	std::basic_string<wchar_t> operator"" _utfw(const char* Source, size_t Length);
	std::basic_string<wchar_t> operator"" _utfw(const char16_t* Source, size_t Length);
	std::basic_string<wchar_t> operator"" _utfw(const char32_t* Source, size_t Length);

	std::shared_ptr<JsonValue> operator"" _json(const char* Src, size_t Length);
}

JSONCPP_NAMESPACE_END