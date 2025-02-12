#pragma once

#include <windows.h>

#define KB *1024
#define MB *(1024 KB)
#define GB *(1024 MB)

typedef struct _ring_buffer { PVOID BaseBuffer;
  SIZE_T BufferSize;
  SIZE_T ReadOffset;
  SIZE_T WriteOffset;
} RING_BUFFER, *PRING_BUFFER;

BOOL CreateRingBuffer(PRING_BUFFER pRingBuffer, SIZE_T Size);
void FreeRingBuffer(PRING_BUFFER pRingBuffer);
BOOL RingBufferHandleRead(PRING_BUFFER pRingBuffer, HANDLE hRead);

#define RINGBUF_READ_PTR(pRingBuffer) ((pRingBuffer)->BaseBuffer + (pRingBuffer)->ReadOffset)
