#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000006 // NTDDI_WIN10_RS5
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // _WIN32_WINNT_WIN10
#include <windows.h>
#include <wincon.h>
#include <assert.h>
#include "create_process.h"

BOOL CreateAsyncPipe(PHANDLE phRead, PHANDLE phWrite, PCSTR PipeName);

/// Create a child process with async io pipes handles
void SpawnChildProcess(LPCSTR lpAppName, PHANDLE phProcess, PHANDLE phStdinWrite, PHANDLE phStdoutRead, HANDLE *phPC)
{
  HANDLE hStdinRead, hStdinWrite;
  HANDLE hStdoutRead, hStdoutWrite;

  assert(CreateAsyncPipe(&hStdinRead, &hStdinWrite, "\\\\.\\pipe\\ptherm_stdin"));
  assert(CreateAsyncPipe(&hStdoutRead, &hStdoutWrite, "\\\\.\\pipe\\ptherm_stdout"));

  COORD size = {800, 600};
  assert(SUCCEEDED(CreatePseudoConsole(size, hStdinRead, hStdoutWrite, 0, phPC)));

  STARTUPINFOEX sInfo = {};
  sInfo.StartupInfo.cb = sizeof(STARTUPINFOEX);
  sInfo.StartupInfo.hStdInput = hStdinRead;
  sInfo.StartupInfo.hStdOutput = hStdoutWrite;
  sInfo.StartupInfo.hStdError = hStdoutWrite;

  size_t bytesRequired;
  InitializeProcThreadAttributeList(NULL, 1, 0, &bytesRequired);

  sInfo.lpAttributeList = HeapAlloc(GetProcessHeap(), 0, bytesRequired);
  assert(sInfo.lpAttributeList != NULL);

  assert(InitializeProcThreadAttributeList(sInfo.lpAttributeList, 1, 0, &bytesRequired));

  assert(UpdateProcThreadAttribute(
      sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, (HPCON)*phPC, sizeof(HPCON), NULL, NULL));

  PROCESS_INFORMATION pInfo = {};

  assert(CreateProcess(
      lpAppName, NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sInfo.StartupInfo, &pInfo));

  *phStdinWrite = hStdinWrite;
  *phStdoutRead = hStdoutRead;
  *phProcess = pInfo.hProcess;
};

BOOL CreateAsyncPipe(PHANDLE phRead, PHANDLE phWrite, PCSTR PipeName)
{
  SECURITY_ATTRIBUTES sAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
  HANDLE hRead = CreateNamedPipeA(PipeName,
                                  PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                                  PIPE_TYPE_BYTE | PIPE_NOWAIT | PIPE_REJECT_REMOTE_CLIENTS,
                                  1,
                                  0,
                                  4096,
                                  0,
                                  &sAttr);
  if (hRead == INVALID_HANDLE_VALUE)
    return FALSE;

  HANDLE hWrite = CreateFileA(PipeName, FILE_GENERIC_WRITE, 0, &sAttr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if (hWrite == INVALID_HANDLE_VALUE)
  {
    CloseHandle(hRead);
    return FALSE;
  }

  *phRead = hRead;
  *phWrite = hWrite;

  return TRUE;
}
