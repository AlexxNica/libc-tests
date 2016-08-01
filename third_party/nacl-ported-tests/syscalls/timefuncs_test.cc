// Copyright (c) 2012 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*
 * These are the syscalls being tested:
 *   #define NACL_sys_gettimeofday           40
 *   #define NACL_sys_clock                  41
 *   #define NACL_sys_nanosleep              42
*/


#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>   // For gettimeofday.
#include <sys/times.h>  // For clock which uses times
#include <time.h>       // For nanosleep.
#include <unistd.h>
#include "gtest/gtest.h"

/*
 * These four definitions are copied from nanosleep_test.c
 */
#define NANOS_PER_MICRO   (1000)
#define MICROS_PER_MILLI  (1000)
#define NANOS_PER_MILLI   (NANOS_PER_MICRO * MICROS_PER_MILLI)
#define MICROS_PER_UNIT   (1000 * 1000)

#define MAX_COUNTER 100000

namespace {

class TimeFuncTests : public ::testing::TestWithParam<struct timespec> {
 protected:

  TimeFuncTests() {
    // You can do set-up work for each test here.
  }

  ~TimeFuncTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



TEST_F(TimeFuncTests, TestClockFunction) {

  clock_t clock_time = 0;
  int counter = 0;

  // clock returns how much cpu time has been used so far. If the test is fast
  // and/or granularity not very fine, then clock() can return 0 sometimes.
  // It should (eventually) return a non-zero value.
  // This loop will keep calling clock() until it returns non-zero or until the
  // counter is larger than |MAX_COUNTER| (so that we don't hang if clock()
  //  is broken).
  while (counter < MAX_COUNTER && clock_time == 0) {
    clock_time = clock();
    ++counter;
  }
  printf("Called clock.  clock_time=%ld, CLOCKS_PER_SEC=%d\n", clock_time,
         (int) CLOCKS_PER_SEC);
  EXPECT_GT(clock_time, 0);

}


TEST_F(TimeFuncTests, TestTimeFuncs) {
  struct timeval tv1;   // Used by gettimeofday
  EXPECT_EQ(0, gettimeofday(NULL, NULL));

  /*
   * gettimeofday takes two args: timeval and timezone pointers.
   * The use of the timezone structure is obsolete; the tz argument should
   * normally be specified as  NULL.
   */
  EXPECT_EQ(0, gettimeofday(&tv1, NULL));

  struct timespec ts;  // Used by nanosleep.

  ts.tv_sec = 1;
  ts.tv_nsec = 5000000;
  EXPECT_EQ(0, nanosleep(&ts, NULL));   // Sleep 1 second

  struct timeval tv2;
  EXPECT_EQ(0, gettimeofday(&tv2, NULL));   // Get time of day again

  /*
   * Because of our nanosleep call, tv2 should have a later time than tv1
   */
  EXPECT_GT(tv2.tv_sec, tv1.tv_sec);

  struct timeval tv3;
  struct timezone tz;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;

  /*
   * Test gettimeofday using obselete timezone struct pointer
   */
  EXPECT_EQ(0, gettimeofday(&tv3, &tz));  // Get time of day again

  /*
   * The time of day (tv3) should not be earlier than time of day (tv2)
   */
  EXPECT_GE(tv3.tv_sec, tv2.tv_sec);
}

/*
 * Returns failure count.  t_suspend should not be shorter than 1us,
 * since elapsed time measurement cannot possibly be any finer in
 * granularity.  In practice, 1ms is probably the best we can hope for
 * in timer resolution, so even if nanosleep suspends for 1us, the
 * gettimeofday resolution may cause a false failure report.
 */
TEST_P(TimeFuncTests, TestNanoSleep) {
  const struct timespec *t_suspend = &GetParam();
  struct timespec t_remain;
  struct timeval  t_start;
  int             rv;
  struct timeval  t_end;
  struct timeval  t_elapsed;

  printf("%40s: %" PRId64 ".%09ld seconds\n",
         "Requesting nanosleep duration",
         (int64_t) t_suspend->tv_sec,
         t_suspend->tv_nsec);
  t_remain = *t_suspend;
  /*
   * BUG: ntp or other time adjustments can mess up timing.
   * BUG: time-of-day clock resolution may be not be fine enough to
   * measure nanosleep duration.
   */
  EXPECT_NE(-1, gettimeofday(&t_start, NULL));

  while (-1 == (rv = nanosleep(&t_remain, &t_remain)) &&
         EINTR == errno) {
  }
  EXPECT_NE(-1, rv);

  EXPECT_NE(-1, gettimeofday(&t_end, NULL));

  t_elapsed.tv_sec = t_end.tv_sec - t_start.tv_sec;
  t_elapsed.tv_usec = t_end.tv_usec - t_start.tv_usec;
  if (t_elapsed.tv_usec < 0) {
    t_elapsed.tv_usec += MICROS_PER_UNIT;
    t_elapsed.tv_sec -= 1;
  }
  if (t_elapsed.tv_usec >= MICROS_PER_UNIT) {
    printf("Microsecond field too large: %ld\n", t_elapsed.tv_usec);
  }

  printf("%40s: %" PRId64 ".%06ld seconds\n",
         "Actual nanosleep duration",
         (int64_t) t_elapsed.tv_sec,
         t_elapsed.tv_usec);

  /*
   * This is the original check and error message (from nanosleep_test.c),
   * which provides feedback and is more readable.
   */
  if (t_elapsed.tv_sec < t_suspend->tv_sec ||
      (t_elapsed.tv_sec == t_suspend->tv_sec &&
       (NANOS_PER_MICRO * t_elapsed.tv_usec < t_suspend->tv_nsec))) {
    printf("Error: Elapsed time too short!"
           " t_elapsed.tv_sec=%" PRId64 " "
           " t_suspend->tv_sec=%" PRId64 " "
           " t_elapsed.tv_usec=%" PRId64 " "
           " t_suspend->tv_nsec=%" PRId64 " \n",
           (int64_t) t_elapsed.tv_sec, (int64_t) t_suspend->tv_sec,
           (int64_t) t_elapsed.tv_usec, (int64_t) t_suspend->tv_nsec);
  }

  /*
   * This check works with BEGIN_TEST/END_TEST and restates the check above
   */
  EXPECT_TRUE(!(t_elapsed.tv_sec < t_suspend->tv_sec ||
          (t_elapsed.tv_sec == t_suspend->tv_sec &&
          (NANOS_PER_MICRO * t_elapsed.tv_usec < t_suspend->tv_nsec))));
}

struct timespec t_suspend[] = {
    { 0,   1 * NANOS_PER_MILLI, },
    { 0,   2 * NANOS_PER_MILLI, },
    { 0,   5 * NANOS_PER_MILLI, },
    { 0,  10 * NANOS_PER_MILLI, },
    { 0,  25 * NANOS_PER_MILLI, },
    { 0,  50 * NANOS_PER_MILLI, },
    { 0, 100 * NANOS_PER_MILLI, },
    { 0, 250 * NANOS_PER_MILLI, },
    { 0, 500 * NANOS_PER_MILLI, },
    { 1,   0 * NANOS_PER_MILLI, },
    { 1, 500 * NANOS_PER_MILLI, },
};

INSTANTIATE_TEST_CASE_P(NanoSleep,
                        TimeFuncTests,
                        ::testing::ValuesIn(t_suspend));

