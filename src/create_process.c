/*
 * @Author: PaNDa2code
 * @Email:  moaaz0688@gmail.com
 * @Date:   2025-02-05 07:06
 * @Description: Code for initializing the child shell program process.
 */

#include <windows.h>
#include <wincon.h>
#include <assert.h>
#include "create_process.h"

#define KB *1024
#define MB *(1024 KB)
#define GB *(1024 MB)

#define PIPE_BUFFER_SIZE 16 MB
#define DEFAULT_CONSOLE_SIZE {800, 600}

BOOL CustomCreatePipe(PHANDLE phRead, PHANDLE phWrite, PCWSTR PipeName);

// Create a child process with async io pipes handles.
// TODO: error handling
void SpawnChildProcess(LPCWSTR lpAppName, PHANDLE phProcess, PHANDLE phStdinWrite, PHANDLE phStdoutRead, HPCON *phPC)
{
  HANDLE hStdinRead, hStdinWrite;
  HANDLE hStdoutRead, hStdoutWrite;

  assert(CustomCreatePipe(&hStdinRead, &hStdinWrite, L"\\\\.\\pipe\\ptherm_stdin"));
  assert(CustomCreatePipe(&hStdoutRead, &hStdoutWrite, L"\\\\.\\pipe\\ptherm_stdout"));

  COORD size = DEFAULT_CONSOLE_SIZE;
  assert(SUCCEEDED(CreatePseudoConsole(size, hStdinRead, hStdoutWrite, 0, phPC)));

  STARTUPINFOEXW sInfo = {};
  sInfo.StartupInfo.cb = sizeof(STARTUPINFOEXW);

  size_t bytesRequired;
  InitializeProcThreadAttributeList(NULL, 1, 0, &bytesRequired);

  sInfo.lpAttributeList = HeapAlloc(GetProcessHeap(), 0, bytesRequired);
  assert(sInfo.lpAttributeList != NULL);

  assert(InitializeProcThreadAttributeList(sInfo.lpAttributeList, 1, 0, &bytesRequired));

  assert(UpdateProcThreadAttribute(sInfo.lpAttributeList,
                                   0,
                                   PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                   (HPCON)*phPC,
                                   sizeof(HPCON),
                                   NULL,
                                   NULL));

  PROCESS_INFORMATION pInfo = {};

  assert(CreateProcessW(lpAppName,
                        NULL,
                        NULL,
                        NULL,
                        TRUE,
                        EXTENDED_STARTUPINFO_PRESENT,
                        NULL,
                        NULL,
                        &sInfo.StartupInfo,
                        &pInfo));

  *phStdinWrite = hStdinWrite;
  *phStdoutRead = hStdoutRead;
  *phProcess = pInfo.hProcess;

  CloseHandle(hStdinRead);
  CloseHandle(hStdoutWrite);
  HeapFree(GetProcessHeap(), 0, sInfo.lpAttributeList);
};

BOOL CustomCreatePipe(PHANDLE phRead, PHANDLE phWrite, PCWSTR PipeName)
{
  if (!phRead || !phWrite || !PipeName)
    return FALSE;

  const PCWSTR PipeNamePrefix = L"\\\\.\\pipe\\";

  if (wcsncmp(PipeName, PipeNamePrefix, wcslen(PipeNamePrefix)))
    return FALSE;

  SECURITY_ATTRIBUTES sAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
  HANDLE hRead = CreateNamedPipeW(PipeName,
                                  PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                                  PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                                  1,
                                  0,
                                  PIPE_BUFFER_SIZE,
                                  100,
                                  &sAttr);
  if (hRead == INVALID_HANDLE_VALUE)
    return FALSE;

  HANDLE hWrite = CreateFileW(PipeName, FILE_GENERIC_WRITE, 0, &sAttr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if (hWrite == INVALID_HANDLE_VALUE)
  {
    CloseHandle(hRead);
    return FALSE;
  }

  *phRead = hRead;
  *phWrite = hWrite;

  return TRUE;
}
