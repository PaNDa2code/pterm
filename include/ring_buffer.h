#pragma once

#include <windows.h>

typedef struct _rw
{
} RW, *PRW;

typedef struct _ring_buffer
{
  SIZE_T start;
  SIZE_T end;
  PVOID BaseBuffer;
  SIZE_T BufferSize;
  LPVOID pSecondaryView;
  PRW rw;
} RING_BUFFER, *PRING_BUFFER;

BOOL CreateRingBuffer(PRING_BUFFER pRingBuffer, SIZE_T Size);
void FreeRingBuffer(PRING_BUFFER pRingBuffer);
BOOL UpdateBuffer(PRING_BUFFER pRingBuffer, HANDLE hRead);
