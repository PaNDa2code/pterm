#include "munit.h"
#include "ring_buffer.h"

MunitResult test_ring_buffer_create(const MunitParameter params[], void *user_data_or_fixture)
{
  const unsigned long size = 64 KB;
  RING_BUFFER ring_buffer = {};
  unsigned char *buffer = malloc(size);
  if (buffer == NULL)
    return MUNIT_SKIP;
  munit_assert_true(CreateRingBuffer(&ring_buffer, size));
  munit_rand_memory(size, buffer);
  memcpy(ring_buffer.BaseBuffer + size / 2, buffer, size);
  munit_assert_memory_equal(size, ring_buffer.BaseBuffer + size / 2, buffer);
  FreeRingBuffer(&ring_buffer);
  free(buffer);
  return MUNIT_OK;
};

MunitResult test_ring_buffer_write(const MunitParameter params[], void *user_data_or_fixture)
{
  const unsigned long size = 64 KB;
  RING_BUFFER ring_buffer = {};
  unsigned char *buffer = malloc(size / 2);
  if (buffer == NULL)
    return MUNIT_SKIP;
  if (!CreateRingBuffer(&ring_buffer, size))
    return MUNIT_ERROR;
  for (int i = 0; i < 10; i++)
    munit_assert_true(RingBufferWrite(&ring_buffer, buffer, size / 2));
  return MUNIT_OK;
}
