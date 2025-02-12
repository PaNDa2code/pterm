#include <string.h>
#include <windows.h>
#include <stdio.h>
#include "create_process.h"
#include "ring_buffer.h"

void dumpBuffer(PVOID Buffer, SIZE_T size, SIZE_T displayBytes);

int main()
{
  RING_BUFFER RingBuffer = {};
  CHAR WriteBuffer[256];
  HANDLE hWrite, hRead;

  for (int i = 0; i < 256; i++)
  {
    WriteBuffer[i] = i;
  }

  CreatePipe(&hRead, &hWrite, NULL, 0);

  CreateRingBuffer(&RingBuffer, 64 KB);

  for (int i = 0; i < 500; i++)
  {
    WriteFile(hWrite, WriteBuffer, 256, NULL, NULL);
    RingBufferHandleRead(&RingBuffer, hRead);
  }

  PUCHAR ReadPtr = RINGBUF_READ_PTR(&RingBuffer); // Get the read pointer

  dumpBuffer(ReadPtr, RingBuffer.BufferSize, 2 KB);

  return 0;
}

void dumpBuffer(PVOID Buffer, SIZE_T size, SIZE_T displayBytes)
{
  displayBytes /= 2;
  for (SIZE_T i = 0; i < size; i++)
  {
    if (i == displayBytes)
    {
      i = size - displayBytes;
      printf("\n");
      for (int j = 0; j < 111; j++)
        printf("+");
    }
    if (i % 32 == 0)
    {
      if (i > 0)
        printf("\n");
      printf("0x%08llx\t", i);
    }
    printf("%02X ", ((PUCHAR)Buffer)[i]);
  }
}
