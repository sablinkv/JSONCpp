#pragma once

//
#define JSON_STRINGIFY(x) JSON_GET_STRINGIFY(x)
#define JSON_GET_STRINGIFY(x) #x

//
#ifndef JSONCPP_NAMESPACE
#define JSONCPP_NAMESPACE JSONCpp
#endif // !JSONCPP_NAMESPACE

#ifndef JSONCPP_NAMESPACE_BEGIN
#define JSONCPP_NAMESPACE_BEGIN namespace JSONCPP_NAMESPACE {
#endif // !JSONCPP_NAMESPACE_BEGIN

#ifndef JSONCPP_NAMESPACE_END
#define JSONCPP_NAMESPACE_END }
#endif // !JSONCPP_NAMESPACE_END

//
#ifdef JSON_USE_EXCEPTION
#include <exception>
	#define JSON_EXCEPTION 1
#else
	#define JSON_EXCEPTION 0
#endif // JSON_USE_EXCEPTION

//
#ifdef JSON_DLL_BUILD
	#if defined(_MSC_VER) || defined(__MINGW32__)
		#define JSON_API __declspec(dllexport)
	#elif defined(__GNUC__) || defined(__clang__)
		#define JSON_API __attribute__((visibility("default")))
	#endif
#elif defined(JSON_DLL)
	#if defined(_MSC_VER) || defined(__MINGW32__)
		#define JSON_API __declspec(dllimport)
	#endif // !JSON_DLL
#endif // JSON_DLL_BUILD

#ifndef JSON_API
	#define JSON_API
#endif // JSON_API

//
#if (__cplusplus >= 201703L)
	#define JSON_HAS_CPP17 1
#else
	#define JSON_HAS_CPP17 0
#endif

//
#if JSON_HAS_CPP17
	#define JSON_NODISCARD [[nodiscard]]
	#define JSON_FALLTHROUGH [[fallthrough]]
#else
	#define JSON_NODISCARD
	#define JSON_FALLTHROUGH
#endif // JSON_HAS_CPP17

//
#if JSON_HAS_CPP17
	#define JSON_IF_CONSTEXPR if constexpr
#else
	#define JSON_IF_CONSTEXPR if
#endif // JSON_HAS_CPP17

//
#ifndef JSON_ASSERT
#include <cassert>
	#define JSON_ASSERT(cond) assert((cond))
#endif // !JSON_ASSERT

//
#ifndef JSON_STATIC_ASSERT_MESSAGE
	#define JSON_STATIC_ASSERT_MESSAGE(cond, msg) static_assert(cond, msg)
#endif // !JSON_STATIC_ASSERT_MESSAGE

#ifndef JSON_STATIC_ASSERT
	#if JSON_HAS_CPP17
		#define JSON_STATIC_ASSERT(cond) static_assert(cond)
	#else
		#define JSON_STATIC_ASSERT(x) static_assert(x, JSON_STRINGIFY(x))
	#endif // JSON_HAS_CPP17
#endif // !JSON_STATIC_ASSERT

//
#if JSON_EXCEPTION
	#define JSON_ASSERT_MESSAGE(cond, msg, ...)						\
			do {													\
				if (!(cond))										\
					throw JSONCpp::JsonException{msg, __VA_ARGS__};	\
			} while (false)

#else
	#define JSON_ASSERT_MESSAGE(cond, msg, ...)															\
        do {																							\
            if (!(cond))																				\
                JSONCpp::ReportError{JSON_GET_STRINGIFY(cond), __FILE__, __LINE__, msg, __VA_ARGS__};	\
        } while(false)

#endif // JSON_EXCEPTION