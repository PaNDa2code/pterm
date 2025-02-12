// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2#examples

#include <windows.h>
#include <stdio.h>

#include "ring_buffer.h"

// This function creates a ring buffer by allocating a pagefile-backed section
// and mapping two views of that section next to each other. This way if the
// last record in the buffer wraps it can still be accessed in a linear fashion
// using its base VA.
BOOL CreateRingBuffer(PRING_BUFFER pRingBuffer, SIZE_T Size)
{
  BOOL result = FALSE;
  HANDLE section = NULL;
  SYSTEM_INFO sysInfo;
  void *ringBuffer = NULL;
  void *placeholder1 = NULL;
  void *placeholder2 = NULL;
  void *view1 = NULL;
  void *view2 = NULL;

  GetSystemInfo(&sysInfo);

  if (Size % sysInfo.dwAllocationGranularity != 0)
  {
    return FALSE;
  }

  placeholder1
      = (PCHAR)VirtualAlloc2(NULL, NULL, 2 * Size, MEM_RESERVE | MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, NULL, 0);

  if (placeholder1 == NULL)
  {
    fprintf(stderr, "VirtualAlloc2 failed, error %lx\n", GetLastError());
    goto Exit;
  }

  result = VirtualFree(placeholder1, Size, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);

  if (result == FALSE)
  {
    fprintf(stderr, "VirtualFreeEx failed, error %lx\n", GetLastError());
    goto Exit;
  }

  placeholder2 = (void *)((ULONG_PTR)placeholder1 + Size);

  section = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, Size, NULL);

  if (section == NULL)
  {
    fprintf(stderr, "CreateFileMapping failed, error %lx\n", GetLastError());
    goto Exit;
  }

  view1 = MapViewOfFile3(section, NULL, placeholder1, 0, Size, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0);

  if (view1 == NULL)
  {
    fprintf(stderr, "MapViewOfFile3 failed, error %lx\n", GetLastError());
    goto Exit;
  }

  placeholder1 = NULL;

  view2 = MapViewOfFile3(section, NULL, placeholder2, 0, Size, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0);

  if (view2 == NULL)
  {
    fprintf(stderr, "MapViewOfFile3 failed, error %lx\n", GetLastError());
    goto Exit;
  }

  pRingBuffer->BaseBuffer = view1;
  pRingBuffer->BufferSize = Size;
  pRingBuffer->pSecondView = view2;

  placeholder2 = NULL;
  view1 = NULL;
  view2 = NULL;

  result = TRUE;

Exit:

  if (section != NULL)
  {
    CloseHandle(section);
  }

  if (placeholder1 != NULL)
  {
    VirtualFree(placeholder1, 0, MEM_RELEASE);
  }

  if (placeholder2 != NULL)
  {
    VirtualFree(placeholder2, 0, MEM_RELEASE);
  }

  if (view1 != NULL)
  {
    UnmapViewOfFileEx(view1, 0);
  }

  if (view2 != NULL)
  {
    UnmapViewOfFileEx(view2, 0);
  }

  return result;
}
