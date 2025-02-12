#include <windows.h>
#include <stdio.h>
#include "create_process.h"

int main()
{
  BOOL result;
  DWORD BytesRead = 0;
  CHAR buffer[1024];
  OVERLAPPED ol = {};

  HANDLE hStdinWrite, hStdoutRead, hProcess;
  HPCON hPC;

  SpawnChildProcess(L"C:\\windows\\system32\\cmd.exe", &hProcess, &hStdinWrite, &hStdoutRead, &hPC);

  ol.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

  PCHAR command = "dir\r\n";

  result = WriteFile(hStdinWrite, command, strlen(command), NULL, &ol);

  WaitForSingleObject(ol.hEvent, INFINITE);
  GetOverlappedResult(hStdinWrite, &ol, &BytesRead, FALSE);

  ResetEvent(ol.hEvent);

  for (int i = 0; i < 2; i++)
  {
    ReadFile(hStdoutRead, buffer, sizeof(buffer), &BytesRead, &ol);
    WaitForSingleObject(ol.hEvent, INFINITE);
    GetOverlappedResult(hStdoutRead, &ol, &BytesRead, FALSE);
    ResetEvent(ol.hEvent);
    printf("%.*s -------->%d\n", (int)BytesRead, buffer, i);
  }

  printf("\n------------ END -------------\n");

  ClosePseudoConsole(hPC);
  CloseHandle(ol.hEvent);
  return 0;
}
