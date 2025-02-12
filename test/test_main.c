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

MunitTest tests[] = {
    {"/CreateRingBuffer", test_ring_buffer_create, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/pterm-tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *const *argv)
{
  return munit_suite_main(&suite, NULL, argc, argv);
}
