#pragma once
#include "config.h"
#include <cstdint>

JSONCPP_NAMESPACE_BEGIN

enum class JsonType : uint32_t
{
    Unknown	= 0,
    Null	= 1,
    Boolean	= 2,
    Number	= 3,
    String	= 4,
    Array	= 5,
    Object	= 6,
};

enum class JsonTokenType : uint32_t
{
    EndFile     = 0,
    Comma       = (1 << 0),	// ,
    Colon       = (1 << 1),	// :
    ArrayBegin  = (1 << 2),	// [
    ArrayEnd    = (1 << 3),	// ]
    ObjectBegin = (1 << 4),	// {
    ObjectEnd   = (1 << 5),	// }
    Null        = (1 << 6),
    True        = (1 << 7),
    False       = (1 << 8),
    Boolean     = True | False,
    Number      = (1 << 9),
    String      = (1 << 10),
    AnyValue    = Null | Boolean | Number | String | ArrayBegin | ObjectBegin
};

JSONCPP_NAMESPACE_END