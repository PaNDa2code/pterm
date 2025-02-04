#include <string.h>
#include <windows.h>
#include <stdio.h>
#include "create_process.h"

int main()
{
  DWORD BytesRead = 0;
  UCHAR buffer[1024] = {};
  HANDLE hStdinWrite, hStdoutRead, hProcess;
  HANDLE hPC;
  SpawnChildProcess("C:\\windows\\system32\\cmd.exe", &hProcess, &hStdinWrite, &hStdoutRead, &hPC);
  Sleep(1000);
  char *command = "exit\n";
  WriteFile(hStdinWrite, command, strlen(command), NULL, NULL);
  ReadFile(hStdoutRead, buffer, sizeof(buffer), &BytesRead, NULL);
  for (SIZE_T i = 0; i < BytesRead; i++)
  {
    printf("0x%X, ", buffer[i]);
  }
  WaitForSingleObject(hProcess, INFINITE);
  return 0;
}
