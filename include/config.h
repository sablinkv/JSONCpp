#pragma once

// 
#define JSON_STRINGIFY(x) JSON_GET_STRINGIFY(x)
#define JSON_GET_STRINGIFY(x) #x

// 
#ifndef JSONCPP_NAMESPACE
#define JSONCPP_NAMESPACE JSONCpp
#endif // JSONCPP_NAMESPACE

#ifndef JSONCPP_NAMESPACE_BEGIN
#define JSONCPP_NAMESPACE_BEGIN namespace JSONCPP_NAMESPACE {
#endif // JSONCPP_NAMESPACE_BEGIN

#ifndef JSONCPP_NAMESPACE_END
#define JSONCPP_NAMESPACE_END }
#endif // JSONCPP_NAMESPACE_END

//
#ifdef JSON_DLL_BUILD
	#if defined(_MSC_VER) || defined(__MINGW32__)
		#define JSON_API __declspec(dllexport)
	#elif defined(__GNUC__) || defined(__clang__)
		#define JSON_API __attribute__((visibility("default")))
	#endif
#else
	#if defined(_MSC_VER) || defined(__MINGW32__)
		#define JSON_API __declspec(dllimport)
	#elif defined(__GNUC__) || defined(__clang__)
		#define JSON_API __attribute__((visibility("hidden")))
	#endif
#endif // JSON_DLL_BUILD

#ifndef JSON_API
	#define JSON_API
#endif // JSON_API

//
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(fallthrough)
		#define JSON_FALLTHROUGH [[fallthrough]]
	#else
		#define JSON_FALLTHROUGH
	#endif // __has_cpp_attribute(fallthrough)
	
	#if __has_cpp_attribute(nodiscard)
		#define JSON_NODISCARD [[nodiscard]]
	#else
		#define JSON_NODISCARD
	#endif // __has_cpp_attribute(nodiscard)

#endif // __has_cpp_attribute

//
#ifndef JSON_ASSERT
#include <cassert>
	#define JSON_ASSERT(cond) assert((cond))
#endif // JSON_ASSERT

#ifndef JSON_STATIC_ASSERT
	#if (defined(__GNUC__) && __GNUC__ >= 6) || (defined(_MSC_VER) && _MSC_VER >= 1910)
		#define JSON_STATIC_ASSERT(cond) static_assert(cond)
	#else
		#define JSON_STATIC_ASSERT(cond) static_assert(cond, JSON_STRINGIFY(cond))
	#endif
#endif // JSON_STATIC_ASSERT

//
#ifdef JSON_USE_EXCEPTION
#include <exception>
	#define JSON_ASSERT_MESSAGE(cond, msg, ...)						\
			do {													\
				if (!(cond))										\
					throw JSONCpp::JsonException{msg, __VA_ARGS__};	\
			} while (false)

#else
	#define JSON_ASSERT_MESSAGE(cond, msg, ...)																\
        do {																								\
            if (!(cond))																					\
                JSONCpp::JsonReportError{JSON_GET_STRINGIFY(cond), __FILE__, __LINE__, msg, __VA_ARGS__};	\
        } while(false)

#endif // JSON_USE_EXCEPTION
