#pragma once

#include <windows.h>

typedef struct _ring_buffer
{
  PVOID BaseBuffer;
  SIZE_T BufferSize;
  SIZE_T ReadOffset;
  SIZE_T WriteOffset;
} RING_BUFFER, *PRING_BUFFER;

BOOL CreateRingBuffer(PRING_BUFFER pRingBuffer, SIZE_T Size);
void FreeRingBuffer(PRING_BUFFER pRingBuffer);
BOOL UpdateRingBuffer(PRING_BUFFER pRingBuffer, HANDLE hRead);
