#pragma once
#include "error.h"
#include <sstream>
#include <iterator>

JSONCPP_NAMESPACE_BEGIN

template<class CharT>
struct UtfTraits {};

template<>
struct UtfTraits<char>
{
	using CharType = char;

	static int Length(const CharType& Char)
	{
		if ((Char & 0x80U) == 0x00U) return 1;
		else if ((Char & 0xE0U) == 0xC0U) return 2;
		else if ((Char & 0xF0U) == 0xE0U) return 3;
		else if ((Char & 0xF8U) == 0xF0U) return 4;
		return -1;
	}

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		if (CodePoint < 0x80U)
		{
			*Dest = (static_cast<CharType>(CodePoint));
		}
		else if (CodePoint < 0x800U)
		{
			*Dest = (static_cast<CharType>(0xC0U | ((CodePoint >> 6U) & 0xFFU)));
			*Dest = (static_cast<CharType>(0x80U | (CodePoint & 0x3FU)));
		}
		else if (CodePoint < 0x10000U)
		{
			*Dest = (static_cast<CharType>(0xE0U | ((CodePoint >> 12U) & 0xFFU)));
			*Dest = (static_cast<CharType>(0x80U | ((CodePoint >> 6U) & 0x3FU)));
			*Dest = (static_cast<CharType>(0x80U | (CodePoint & 0x3FU)));
		}
		else
		{
			JSON_ASSERT_MESSAGE(CodePoint < 0x110000U, "Invalid UTF8 codepoint.");
			*Dest = (static_cast<CharType>(0xF0U | ((CodePoint >> 18U) & 0xFFU)));
			*Dest = (static_cast<CharType>(0x80U | ((CodePoint >> 12U) & 0x3FU)));
			*Dest = (static_cast<CharType>(0x80U | ((CodePoint >> 6U) & 0x3FU)));
			*Dest = (static_cast<CharType>(0x80U | ((CodePoint & 0x3FU))));
		}
		return Dest;
	}

	template<class InIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		if (First == Last)
			return First;

		auto Char = *First++;
		auto Byte = Length(Char);

		JSON_ASSERT_MESSAGE(Byte > 0, "Invalid UTF8 sequence.");

		switch (Byte)
		{
		case 1:
			CodePoint = static_cast<uint32_t>(Char);
			break;
		case 2:
			CodePoint = (static_cast<uint8_t>(Char) & 0x1FU) << 6U;
			Char = *First++;
			CodePoint |= static_cast<uint8_t>(Char) & 0x3FU;
			break;
		case 3:
			CodePoint = (static_cast<uint8_t>(Char) & 0x1FU) << 12U;
			Char = *First++;
			CodePoint |= (static_cast<uint8_t>(Char) & 0x3FU) << 6U;
			Char = *First++;
			CodePoint |= static_cast<uint8_t>(Char) & 0x3FU;
			break;
		case 4:
			CodePoint = (static_cast<uint8_t>(Char) & 0x0FU) << 18U;
			Char = *First++;
			CodePoint |= (static_cast<uint8_t>(Char) & 0x3FU) << 12U;
			Char = *First++;
			CodePoint |= (static_cast<uint8_t>(Char) & 0x3FU) << 6U;
			Char = *First++;
			CodePoint |= static_cast<uint8_t>(Char) & 0x3FU;
			break;
		}
		return First;
	}
};

template<>
struct UtfTraits<char16_t>
{
	using CharType = char16_t;

	static int Length(const CharType& Char)
	{
		if (Char < 0xD800U || Char > 0xDFFFU)
			return 1;
		else if (Char < 0xDC00U)
			return 2;
		return -1;
	}

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		if (CodePoint < 0x10000U)
		{
			JSON_ASSERT_MESSAGE(CodePoint < 0xD800U || CodePoint > 0xDFFFU, "Invalid UTF16 codepoint.");
			*Dest = (static_cast<CharType>(CodePoint));
		}
		else
		{
			JSON_ASSERT_MESSAGE(CodePoint < 0x110000U, "Invalid UTF16 codepoint.");
			CodePoint -= 0x10000U;
			*Dest = (static_cast<CharType>(0xD800U | (CodePoint >> 10U)));
			*Dest = (static_cast<CharType>(0xDC00U | (CodePoint & 0x3FFU)));
		}
		return Dest;
	}

	template<class InIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		if (First == Last)
			return First;

		auto Char = *First++;
		auto Byte = Length(Char);

		JSON_ASSERT_MESSAGE(Byte > 0, "Invalid UTF16 sequence.");

		switch (Byte)
		{
		case 1:
			CodePoint = static_cast<uint32_t>(Char);
			break;
		case 2:
			CodePoint = (static_cast<uint32_t>(Char) & 0x3FFU) << 10U;
			Char = *First++;
			JSON_ASSERT((Char > 0xDBFFU && Char < 0xE000U));
			CodePoint |= (static_cast<uint32_t>(Char) & 0x3FFU);
			CodePoint += 0x10000U;
			break;
		}
		return First;
	}
};

template<>
struct UtfTraits<char32_t>
{
	using CharType = char32_t;

	static int Length(const CharType& Char)
	{
		return (Char < 0x110000U ? 1 : -1);
	}

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		JSON_ASSERT_MESSAGE(CodePoint < 0x110000U, "Invalid UTF32 codepoint.");
		*Dest = (static_cast<CharType>(CodePoint));
		return Dest;
	}

	template<class InIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		if (First == Last)
			return First;
		auto Char = *First++;
		JSON_ASSERT_MESSAGE(Length(Char) > 0, "Invalid UTF32 sequence.");
		CodePoint = static_cast<uint32_t>(Char);

		return First;
	}
};

template<>
struct UtfTraits<wchar_t>
{
	using CharType = wchar_t;
	using TraitsType = std::conditional<sizeof(CharType) < sizeof(char32_t), UtfTraits<char16_t>, UtfTraits<char32_t>>::type;

	static int Length(const CharType& Char)
	{
		return TraitsType::Length(Char);
	}

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		return TraitsType::Encode(CodePoint, Dest);
	}

	template<class InIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		return TraitsType::Decode(First, Last, CodePoint);
	}
};

template<class CharT = char>
struct AsciiTraits
{
	using CharType = CharT;

	static int Length(const CharType& Char)
	{
		return (Char < 0x80U ? 1 : -1);
	}

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		JSON_ASSERT_MESSAGE(CodePoint < 0x80U, "Invalid ASCII codepoint.");
		*Dest = static_cast<CharType>(CodePoint & 0xFFU);
		return Dest;
	}

	template<class InIter, class OutIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		if (First == Last)
			return Last;
		auto Char = *First++;
		JSON_ASSERT_MESSAGE(Length(Char) > 0, "Invalid ASCII sequence.");
		CodePoint = static_cast<uint8_t>(Char);
		return First;
	}
};

template<class CharT, class Traits = UtfTraits<CharT>>
struct Utf
{
	using CharType = CharT;
	using TraitsType = Traits;

	static int Length(const CharType& Char)	{ return TraitsType::Length(Char); }

	template<class OutIter>
	static OutIter Encode(uint32_t CodePoint, OutIter Dest)
	{
		return TraitsType::Encode(CodePoint, Dest);
	}

	static void Encode(uint32_t CodePoint, std::basic_ostream<CharType>& OStream)
	{
		Encode(CodePoint, std::ostream_iterator<CharType, CharType>(OStream));
	}

	static std::basic_string<CharType> Encode(uint32_t CodePoint)
	{
		std::basic_stringstream<CharType> SStream;
		Encode(CodePoint, std::ostream_iterator<CharType, CharType>(SStream));
		return SStream.str();
	}

	template<class InIter>
	static InIter Decode(InIter First, InIter Last, uint32_t& CodePoint)
	{
		return TraitsType::Decode(First, Last, CodePoint);
	}

	template<class InIter>
	static uint32_t Decode(InIter First, InIter Last)
	{
		uint32_t CodePoint;
		Decode(First, Last, CodePoint);
		return CodePoint;
	}

	static void Decode(std::basic_istream<CharType>& IStream, uint32_t& CodePoint)
	{
		using is_iter = std::istream_iterator<CharType, CharType>;
		Decode(is_iter(IStream), is_iter{}, CodePoint);
	}

	static uint32_t Decode(std::basic_istream<CharType>& IStream)
	{
		uint32_t CodePoint;
		Decode(IStream, CodePoint);
		return CodePoint;
	}

	template<class Target, class InIter, class OutIter>
	static OutIter Transcode(InIter First, InIter Last, OutIter Dest)
	{
		while (First != Last)
		{
			uint32_t CodePoint;
			First = Decode(First, Last, CodePoint);
			Utf<Target>::Encode(CodePoint, Dest);
		}
		return Dest;
	}

	template<class Target>
	static void Transcode(std::basic_istream<CharType>& IStream, std::basic_ostream<Target>& OStream)
	{
		using IStreamIter = std::istream_iterator<CharType, CharType>;
		using OStreamIter = std::ostream_iterator<Target, Target>;
		Transcode<Target>(IStreamIter(IStream), IStreamIter{}, OStreamIter(OStream));
	}

	template<class Target>
	static std::basic_string<Target> Transcode(const CharType* Src, size_t Length)
	{
		JSON_ASSERT(Src != nullptr);
		std::basic_stringstream<Target> SStream;
		Transcode<Target>(Src, Src + Length, std::ostream_iterator<Target, Target>(SStream));
		return SStream.str();
	}

	template<class Target>
	static std::basic_string<Target> Transcode(const CharType* Src)
	{
		JSON_ASSERT(Src != nullptr);
		const CharType* Ptr = Src;
		while (*Ptr) ++Ptr;
		uint32_t Length = static_cast<uint32_t>(Ptr - Src);
		return Transcode<Target>(Src, Length);
	}
};

using Utf8		= Utf<char, UtfTraits<char>>;
using Utf16		= Utf<char16_t, UtfTraits<char16_t>>;
using Utf32		= Utf<char32_t, UtfTraits<char32_t>>;
using UtfW		= Utf<wchar_t, UtfTraits<wchar_t>>;
using UtfAscii	= Utf<char, AsciiTraits<char>>;

JSONCPP_NAMESPACE_END