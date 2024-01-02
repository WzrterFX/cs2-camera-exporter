#include <windows.h>

#include <TlHelp32.h>
#include <vector>

using namespace std;

// get procces id
uintptr_t GetProcessID(const wchar_t* processName)
{
    uintptr_t processID = 0;

    HANDLE proccesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (proccesSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W proccesEntry;
    proccesEntry.dwSize = sizeof(proccesEntry);

    if (Process32FirstW(proccesSnapshot, &proccesEntry))
    {
        while (true)
        {
            if (_wcsicmp(proccesEntry.szExeFile, processName) == 0)
            {
                processID = proccesEntry.th32ProcessID;
                break;
            }

            if (!Process32NextW(proccesSnapshot, &proccesEntry))
                break;
        }
    }
    CloseHandle(proccesSnapshot);

    return processID;
}

// read dll or exe memory region
uintptr_t GetModuleBaseAddress(uintptr_t processID, const wchar_t* moduleName)
{
    uintptr_t moduleBaseAddress = 0;

    HANDLE moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (moduleSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    MODULEENTRY32W moduleEntry;
    moduleEntry.dwSize = sizeof(moduleEntry);

    if (Module32FirstW(moduleSnapshot, &moduleEntry))
    {
        while (true)
        {
            if (_wcsicmp(moduleEntry.szModule, moduleName) == 0)
            {
                moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                break;
            }

            if (!Module32NextW(moduleSnapshot, &moduleEntry))
                break;
        }
    }
    CloseHandle(moduleSnapshot);

    return moduleBaseAddress;
}