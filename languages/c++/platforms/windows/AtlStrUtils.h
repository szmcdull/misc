#pragma once
#include <atlstr.h>

template<class T, typename ... Args>
CStringT<T, StrTraitATL<T, ChTraitsCRT<T>>> AtlStrFormat(const T* format, Args ... args)
{
    CStringT<T, StrTraitATL<T, ChTraitsCRT<T>>> result;
    result.Format(format, args...);
    return result;
}

template<typename ... T1, class T, typename ... Args>
CStringT<T1...>& AtlStrFormat(CStringT<T1...> &s, const T* format, Args ... args)
{
    s.Format(format, args...);
    return s;
}