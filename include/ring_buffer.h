#pragma once

#include <windows.h>

typedef struct _ring_buffer
{
  SIZE_T start;
  SIZE_T end;
  PVOID BaseBuffer;
  SIZE_T BufferSize;
  LPVOID pSecondView;
} RING_BUFFER, *PRING_BUFFER;

BOOL CreateRingBuffer(PRING_BUFFER pRingBuffer, SIZE_T Size);

#define KB *1024
#define MB KB KB
