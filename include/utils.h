#pragma once
#include "utf.h"
#include <cstring>

JSONCPP_NAMESPACE_BEGIN

template<class CharT = char>
bool IsWhiteSpace(const CharT& Char) noexcept
{
	return (Char == CharT(' ') || Char == CharT('\t') ||
		Char == CharT('\n') || Char == CharT('\r'));
}

template<class CharT = char>
bool IsSign(const CharT& Char) noexcept
{
	return (Char == CharT('-') || Char == CharT('+'));
}

template<class CharT = char>
bool IsDigit(const CharT& Char) noexcept
{
	return (Char >= CharT('0') && Char <= CharT('9'));
}

template<class CharT = char>
bool IsExp(const CharT& Char) noexcept
{
	return (Char == CharT('e') || Char == CharT('E'));
}

template<class CharT = char>
bool IsHex(const CharT& Char) noexcept
{
	return IsDigit(Char) || (Char >= CharT('a') && Char <= CharT('f') || (Char >= CharT('A') && Char <= CharT('F')));
}

template<class CharT = char>
bool IsJsonNull(const CharT* First, const CharT* Last) noexcept
{
	auto Length = Last - First;
	if (*First == CharT('n') && Length > 3)
	{
		static const std::basic_string<CharT> Null = Utf8::Transcode<CharT>("null", 5);
		return static_cast<bool>(std::memcmp(First, Null.c_str(), sizeof(CharT) * 4) == 0);
	}
	return false;
}

template<class CharT = char>
bool IsJsonBoolean(const CharT* First, const CharT* Last) noexcept
{
	auto Length = Last - First;
	if (*First == CharT('t') && Length > 3)
	{
		static const std::basic_string<CharT> True = Utf8::Transcode<CharT>("true", 5);
		return static_cast<bool>(std::memcmp(First, True.c_str(), sizeof(CharT) * 4) == 0);
	}
	if (*First == CharT('f') && (Last - First) > 4)
	{
		static const std::basic_string<CharT> False = Utf8::Transcode<CharT>("false", 6);
		return static_cast<bool>(std::memcmp(First, False.c_str(), sizeof(CharT) * 5) == 0);
	}
	return false;
}

template<class CharT = char>
bool IsJsonHexNumber(const CharT* First, const CharT* Last) noexcept
{
	auto Length = Last - First;
	if (Length > 2 && *First == CharT('0') && *(First + 1) == CharT('x'))
	{
		auto _First = First + 2;
		while (_First != Last)
		{
			if (!IsHex(*_First++)) return false;
		}
		return _First == Last;
	}
	return false;
}

template<class CharT = char>
bool IsJsonNumber(const CharT& Char) noexcept
{
	return IsSign(Char) || IsDigit(Char) || IsExp(Char);
}

JSONCPP_NAMESPACE_END