#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wininet.h>
#include <atlstr.h>
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>

namespace INet
{
    // Handle
    class Handle
        : public boost::noncopyable
    {
    public:
        inline ~Handle()
        {
            if (m_handle) InternetCloseHandle(m_handle);
        }

        operator HINTERNET() const
        {
            return m_handle;
        }

    protected:
        inline Handle()
            : m_handle(nullptr)
        {}

    protected:
        HINTERNET m_handle;
    };

    class Headers
        : public std::map<CStringA, CStringA>
    {
    public:
        typedef std::map<CStringA, CStringA> inherited;

        Headers()
            : m_modified(false)
        {}
        CStringA GetString();
        CStringA& operator[](const CStringA& k)
        {
            m_modified = true;
            return inherited::operator [](k);
        }
        CStringA& operator[](CStringA&& k)
        {
            m_modified = true;
            return inherited::operator [](std::forward<CStringA>(k));
        }

    protected:
        bool m_modified;
        CStringA m_string;
    };

    // Connection
    class Connection
        : public Handle
    {
    public:

    protected:
        friend class Request;
    };

    // Agent
    class Agent
        : public Handle
    {
    public:
        Agent(const wchar_t* agent = nullptr);
        Agent(const char* agent);

        void SetHeader(const char* key, const char* value);
        void SetHeaders(const Headers& headers);

        std::string Get(CString url);
        std::string Post(const char* url, CStringA data, CStringA contentType = "application/x-www-form-urlencoded");
        void Download(const char* url, LPCTSTR path, bool resume = true);

    protected:
        Headers m_headers;
        friend class Request;

        static std::string GetResponse(HINTERNET conn);
    };

    // Request
    class Request
        : public Handle
    {
    public:
        Request(const Agent* conn, const char* url, CStringA action, Headers& headers = Headers{}, CStringA data = CStringA{});
        long long GetContentLength();
        void GetContentRange(long long & start, long long & end, long long & total);

    protected:
        const Agent& m_agent;
        Connection m_conn;
    };

    class INetException
        : public std::exception
    {
    public:
        INetException();

        virtual char const* what() const;
    protected:
        DWORD err;
    };
}