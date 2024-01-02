#pragma once

#include <windows.h>

uintptr_t GetProcessID(const wchar_t* processName);

uintptr_t GetModuleBaseAddress(uintptr_t processID, const wchar_t* moduleName);