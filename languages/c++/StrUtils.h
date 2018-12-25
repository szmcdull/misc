#pragma once

#if __cpp_lib_filesystem

#include <filesystem>
typedef std::filesystem::path PathType;
namespace _StrUtils
{
    namespace filesystem = std::filesystem;
}

#elif __cpp_lib_experimental_filesystem

#include <experimental/filesystem>
typedef std::experimental::filesystem::path PathType;
namespace _StrUtils
{
    namespace filesystem = std::experimental::filesystem;
}
#else

#include <boost/filesystem.hpp>
typedef boost::filesystem::path PathType;
namespace _StrUtils
{
    namespace filesystem = boost::filesystem;
}
#endif

namespace filesystem = _StrUtils::filesystem;

template<typename C>
PathType GetExeDir(const C* exepath)
{
    return _StrUtils::filesystem::absolute(exepath).parent_path();
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

inline size_t StrLength(const char)
{
    return 1;
}

inline size_t StrLength(const wchar_t)
{
    return 1;
}

template<class T>
bool StrIsDigit(const T& str)
{
    size_t length = StrLength(str);
    for (size_t i = 0; i < length; i++)
    {
        auto c = str[i];
        if (c < '0' || c > '9')
            return false;
    }
    return true;
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

template<class S, class Container>
std::basic_string<typename Container::value_type::value_type> StrJoin(const Container& c, S delimiter)
{
    size_t size = 0, sizeDelimiter = StrLength(delimiter);
    for (auto& s : c)
    {
        size += StrLength(s) + sizeDelimiter;
    }
    if (size)
    {
        size -= sizeDelimiter;
    }

    std::basic_string<Container::value_type> result;
    result.reserve(size);
    for (auto p = c.begin();;)
    {
        result += *p;
        p++;
        if (p == c.end())
            break;
        result += delimiter;
    }

    return result;
}

template<class V, class V2, class V3>
bool InRange(const V& value, const V2& start, const V3& end)
{
    return value >= start && value < end;
}