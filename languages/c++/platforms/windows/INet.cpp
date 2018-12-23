#include "INet.h"
#include <sstream>
#include "../../defer.h"
#include "../../StrUtils.h"
#include "AtlStrUtils.h"
#include <fstream>

#pragma comment(lib, "wininet")

using namespace std;

INet::Agent::Agent(const wchar_t* agent)
{
    m_handle = InternetOpenW(agent, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
}

INet::Agent::Agent(const char * agent)
{
    m_handle = InternetOpenA(agent, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
}

void INet::Agent::SetHeader(const char * key, const char * value)
{
    m_headers[key] = value;
}

void INet::Agent::SetHeaders(const Headers& headers)
{
    m_headers = headers;
}


void INet::Agent::Download(const char * url, LPCTSTR path, bool resume)
{
    Headers headers;
    ofstream outFile;
    outFile.exceptions(ofstream::failbit | ofstream::badbit);

    while (resume)
    {
        long long filesize = 0;
        try
        {
            filesize = filesystem::file_size(path);
        }
        catch (...)
        {

        }

        if (filesize == 0) // 获取文件大小失败，重新下载
        {
            resume = false;
            break;
        }

        // 从文件末尾继续下载
        headers["Range"] = AtlStrFormat("bytes=%lld-", filesize);
        break;
    }

    // 发送请求
    Request req(this, url, "GET", headers);
    long long start, end, total;
    req.GetContentRange(start, end, total);
    
    if (start >= 0)
    {
        outFile.open(path, ios::binary | ios::ate);
    }
    else // 服务器不支持断点续传，重新下载
    {
        outFile.open(path, ios::binary | ios::trunc);
        resume = false;
    }

    char buf[4096];
    DWORD bytesRead;

    while (InternetReadFile(req, buf, sizeof(buf), &bytesRead) && bytesRead > 0)
    {
        if (bytesRead == 0)
        {
            outFile.close();
            return;
        }
        outFile.write(buf, bytesRead);
    }

    throw INetException();
}

std::string INet::Agent::GetResponse(HINTERNET conn)
{
    char buf[4096];
    DWORD bytesRead;
    ostringstream oss;

    while (InternetReadFile(conn, buf, sizeof(buf), &bytesRead))
    {
        if (bytesRead == 0)
        {
            return oss.str();
        }
        oss.write(buf, bytesRead);
    }

    throw INetException();
}

string INet::Agent::Get(CString url)
{
    auto strHeaders = m_headers.GetString();
    HINTERNET h = InternetOpenUrl(m_handle, url, strHeaders, strHeaders.GetLength(), 0, 0);
    if (h == nullptr)
    {
        throw INetException();
    }
    defer{ InternetCloseHandle(h); };
    return GetResponse(h);
}

std::string INet::Agent::Post(const char* url, CStringA data, CStringA contentType)
{
    m_headers["Content-Type"] = contentType;
    Request req(this, url, "POST", m_headers, data);
    defer{ InternetCloseHandle(req); };

    return GetResponse(req);
}

INet::INetException::INetException()
{
    err = GetLastError();
}

char const * INet::INetException::what() const
{
    return "";
}

INet::Request::Request(const Agent* agent, const char * url, CStringA action, Headers& headers, CStringA data)
    : m_agent(*agent)
{
    bool ok = false;
    URL_COMPONENTSA components{sizeof(URL_COMPONENTSA)};
    components.dwHostNameLength = 1;
    components.dwUserNameLength = 1;
    components.dwPasswordLength = 1;
    components.dwUrlPathLength = 1;
    if (!InternetCrackUrlA(url, 0, 0, &components))
    {
        throw INetException();
    }

    CStringA host(components.lpszHostName, components.dwHostNameLength);
    CStringA username(components.lpszUserName, components.dwUserNameLength);
    CStringA password(components.lpszPassword, components.dwPasswordLength);
    m_conn.m_handle = InternetConnectA(m_agent, host, components.nPort, username, password, INTERNET_SERVICE_HTTP, 0, 0);
    if (!m_conn.m_handle)
    {
        throw INetException();
    }
    defer
    {
        if (!ok)
        {
            InternetCloseHandle(m_conn.m_handle);
            m_conn.m_handle = nullptr;
        }
    };

    CStringA path(components.lpszUrlPath, components.dwUrlPathLength);
    DWORD flags = INTERNET_FLAG_NO_UI;
    if (components.nScheme == INTERNET_SCHEME_HTTPS)
        flags |= INTERNET_FLAG_SECURE;
    m_handle = HttpOpenRequestA(m_conn, action, path, nullptr, nullptr, nullptr, flags, 0);
    if (m_conn.m_handle == nullptr)
    {
        throw INetException();
    }

    CStringA strHeaders = headers.GetString();
    if (!HttpSendRequestA(m_handle, strHeaders, strHeaders.GetLength(), (LPVOID)(const char*)data, data.GetLength()))
    {
        throw INetException();
    }

    ok = true;
}

long long INet::Request::GetContentLength()
{
    char lengthStr[20];
    DWORD LengthSize = size(lengthStr);
    if (!HttpQueryInfoA(m_handle, HTTP_QUERY_CONTENT_LENGTH, lengthStr, &LengthSize, 0))
    {
        throw INetException();
    }

    return atoll(lengthStr);
}

void INet::Request::GetContentRange(long long & start, long long & end, long long & total)
{
    start = -1;
    end = -1;
    total = -1;

    char buf[60];
    DWORD length = size(buf);
    if (!HttpQueryInfoA(m_handle, HTTP_QUERY_CONTENT_LENGTH, buf, &length, 0))
    {
        return;
    }

    char first[40], second[20];
    if (sscanf_s(buf, "%s/%s", first, size(first), second, size(second)) <= 0)
    {
        return;
    }

    if (first[0] != '*')
    {
        sscanf_s(first, "%lld-%lld", &start, &end);
    }

    if (second[0] != '*')
    {
        sscanf_s(second, "%lld", &end);
    }
}

CStringA INet::Headers::GetString()
{
    if (!m_modified)
        return m_string;

    m_string.Empty();
    if (!empty())
    {
        for (auto p = begin(); ;)
        {
            m_string += p->first + ": " + p->second;
            p++;
            if (p == end())
            {
                break;
            }
            m_string += "\r\n";
        }
    }

    m_modified = false;
    return m_string;
}
