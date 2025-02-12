#include <windows.h>
#include <stdio.h>
#include "create_process.h"

int main()
{
  DWORD BytesRead = 0;
  CHAR buffer[1024];
  HANDLE hStdinWrite, hStdoutRead, hProcess;
  HANDLE hPC;
  SpawnChildProcess(L"C:\\windows\\system32\\cmd.exe", &hProcess, &hStdinWrite, &hStdoutRead, &hPC);
  PCHAR command = "echo HelloWorld!\r\n";
  WriteFile(hStdinWrite, command, strlen(command), NULL, NULL);
  Sleep(1000);
  ReadFile(hStdoutRead, buffer, sizeof(buffer), &BytesRead, NULL);
  printf("Output: %.*s\n", (int)BytesRead, buffer);
  WriteFile(hStdinWrite, command, strlen(command), NULL, NULL);
  Sleep(1000);
  ReadFile(hStdoutRead, buffer, sizeof(buffer), &BytesRead, NULL);
  printf("Output: %.*s\n", (int)BytesRead, buffer);

  ReadFile(hStdoutRead, buffer, sizeof(buffer), &BytesRead, NULL);
  printf("Output: %.*s\n", (int)BytesRead, buffer);

  ClosePseudoConsole(hPC);
  return 0;
}
