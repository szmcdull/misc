#pragma once
#include <atlstr.h>

CString GetTempDirAsString()
{
    DWORD size = GetTempPath(0, nullptr);
    CString result(wchar_t(0), int(size - 1));
    GetTempPath(size - 1, (LPTSTR)(LPCTSTR)result);
    return result;
}

template<class T>
int NewProcess(T cmdline, DWORD flags = 0, bool wait = false)
{
    int result = 0;
    STARTUPINFO startupinfo{ sizeof(STARTUPINFO) };
    PROCESS_INFORMATION processinfo;
    CString strCmd(cmdline);

    if (!CreateProcess(nullptr, (LPTSTR)(LPCTSTR)strCmd, nullptr, nullptr, FALSE,
        flags | CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &startupinfo, &processinfo))
    {
        return GetLastError();
    }

    if (wait)
    {
        WaitForSingleObject(processinfo.hProcess, INFINITE);
        if (!GetExitCodeProcess(processinfo.hProcess, (LPDWORD)&result))
        {
            result = GetLastError();
        }
    }

    CloseHandle(processinfo.hProcess);
    CloseHandle(processinfo.hThread);
    return result;
}