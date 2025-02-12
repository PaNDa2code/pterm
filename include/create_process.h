#pragma once
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000006 // NTDDI_WIN10_RS5
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // _WIN32_WINNT_WIN10

#include <windows.h>

void SpawnChildProcess(LPCWSTR lpAppName, PHANDLE phProcess, PHANDLE phStdinWrite, PHANDLE phStdoutRead, HANDLE *phPC);
