#pragma once

template <typename T, typename S>
bool is_one_of(const T &first, const S &second)
{
    return first == second;
}
template <typename T, typename S, typename... Args>
bool is_one_of(const T &first, const S &second, Args... args)
{
    return (first == second) || is_one_of(first, args...);
}
