#include "munit.h"

#include "ring_buffer.h"

MunitResult test_ring_buffer_create(const MunitParameter params[], void *user_data_or_fixture);
MunitResult test_ring_buffer_write(const MunitParameter params[], void *user_data_or_fixture);

MunitTest tests[] = {
    {"/CreateRingBuffer", test_ring_buffer_create, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/RingBufferWrite", test_ring_buffer_write, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/pterm-tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *const *argv)
{
  return munit_suite_main(&suite, NULL, argc, argv);
}
