#pragma once
#include "config.h"
#include <stdarg.h>
#include <string>
#include <iostream>

JSONCPP_NAMESPACE_BEGIN

struct JSON_API JsonReportError
{
    static constexpr size_t Size = 1024;

    template<class... Types>
    JsonReportError(const char* Cond, const char* File, unsigned int Line, const char* const Msg, Types ...Args)
    {
        char MsgBuffer[Size];
        GetVaArgs(MsgBuffer, Size, Msg, Args...);
        std::cerr << "Assertion Failed: " << Cond << '\n';
        std::cerr << "Message         : " << MsgBuffer << '\n';
        std::cerr << "File            : " << File << '\n';
        std::cerr << "Line            : " << Line << '\n';
    }

    static int GetVaArgs(char* Buffer, unsigned int MaxLength, const char* Format, ...)
    {
        va_list Args;
        va_start(Args, Format);
        int Length = vsnprintf(Buffer, MaxLength, Format, Args);
        va_end(Args);
        return Length;
    }

    ~JsonReportError() { abort(); }
};

class JSON_API JsonException : public std::exception {
public:
    JsonException(const std::string& Msg) : m_Message(Msg) {}

    template<class ...Types>
    JsonException(const std::string& Msg, Types&& ...Args)
    {
        if (sizeof...(Types) > 0) {
            char Buffer[JsonReportError::Size];
            int Length = JsonReportError::GetVaArgs(Buffer, JsonReportError::Size, Msg.c_str(), Args...);
            m_Message = std::string(Buffer, Length);
        }
        else {
            m_Message = Msg;
        }
    }

    ~JsonException() = default;

    char const* what() const noexcept override { return m_Message.c_str(); }

protected:
    std::string m_Message;
};

JSONCPP_NAMESPACE_END