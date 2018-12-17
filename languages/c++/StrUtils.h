#pragma once
#ifdef _HAS_CXX17
#include <filesystem>
typedef std::filesystem::path PathType;
#else
#include <boost/filesystem.hpp>
typedef boost::filesystem::path PathType;
#endif

template<typename C>
PathType GetExeDir(const C* exepath)
{
    return std::filesystem::absolute(exepath).parent_path();
}

template<typename C>
PathType GetExeName(const C* exepath)
{
    return PathType(exepath).filename();
}

template<typename C, typename ... Args>
std::basic_string<C> StrFormat(const std::basic_string<C>& format, Args ... args)
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...);
    std::basic_string<C> result(size, 0);
    snprintf(result.data(), size, format.c_str(), args ...);
    return result;
}

template<typename ... Args>
std::string StrFormat(const char* format, Args ... args)
{
    size_t size = snprintf(nullptr, 0, format, args ...) + 1;
    std::string result(size, 0);
    snprintf(result.data(), size, format, args ...);
    return result;
}

template<typename ... Args>
std::wstring StrFormat(const wchar_t* format, Args ... args)
{
    size_t size = wcslen(format) * 2;
    size_t r;
    std::wstring result(size, 0);

    while ((r = swprintf(&result[0], size, format, args ...) + 1) < 0)
    {
        size += size;
        result.resize(size);
    }
    result.resize(r);
    return result;
}

template<class C>
size_t StrLength(const C* str);

template<class S>
size_t StrLength(const std::basic_string<S>& str)
{
    return str.length();
}

template<>
inline size_t StrLength<char>(const char* str)
{
    return strlen(str);
}

template<>
inline size_t StrLength<wchar_t>(const wchar_t* str)
{
    return wcslen(str);
}

template<size_t n>
size_t StrLength(const char str[n])
{
    return n - 1;
}

template<size_t n>
size_t StrLength(const wchar_t str[n])
{
    return n - 1;
}

template<class S, class Container>
void StrSplit(S str, int delimiter, Container& c)
{
    c.clear();
    size_t start = 0;
    size_t length = StrLength<>(str);

    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == delimiter)
        {
            c.push_back(Container::value_type(str, start, i-start));
            start = i + 1;
        }
    }
    c.push_back(Container::value_type(str, start, length - start));
}
