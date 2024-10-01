#pragma once

#include "finally.h"
#include "is_one_of.h"

template <typename Enum>
std::underlying_type_t<Enum> to_underlying(Enum e)
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}
