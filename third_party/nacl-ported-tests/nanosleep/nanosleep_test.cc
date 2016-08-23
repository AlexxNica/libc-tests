/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "gtest/gtest.h"

#define NANOS_PER_MICRO   (1000)
#define MICROS_PER_MILLI  (1000)
#define NANOS_PER_MILLI   (NANOS_PER_MICRO * MICROS_PER_MILLI)
#define MICROS_PER_UNIT   (1000 * 1000)

namespace {

class NanoSleepTests : public ::testing::TestWithParam<struct timespec> {
 protected:

  NanoSleepTests() {
    // You can do set-up work for each test here.
  }

  ~NanoSleepTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



/*
 * We don't convert to floating point, so a precondition is that
 * tv_usec is within range, i.e., the timeval has been normalized.
 */
void PrintTimeval(FILE *iob, struct timeval const *tv) {
  fprintf(iob, "%" PRId64".%06ld", (int64_t) tv->tv_sec, tv->tv_usec);
}

/* timespec fields should have been ts_nsec etc */
void PrintTimespec(FILE *iob, struct timespec const *ts) {
  fprintf(iob, "%" PRId64".%09ld", (uint64_t) ts->tv_sec, ts->tv_nsec);
}

void NormalizeTimeval(struct timeval *tv) {
  int first = 1;
  while (tv->tv_usec < 0) {
    tv->tv_usec += MICROS_PER_UNIT;
    tv->tv_sec -= 1;
    if (!first) {
      fprintf(stderr, "NormalizedTimeval: usec too small, 2x normalize!\n");
      PrintTimeval(stderr, tv); putc('\n', stderr);
    }
    first = 0;
  }
  first = 1;
  while (tv->tv_usec >= MICROS_PER_UNIT) {
    tv->tv_usec -= MICROS_PER_UNIT;
    tv->tv_sec += 1;
    if (!first) {
      fprintf(stderr, "NormalizedTimeval: usec too large, 2x normalize!\n");
      PrintTimeval(stderr, tv); putc('\n', stderr);
    }
    first = 0;
  }
}

/*
 * Returns failure count.  t_suspend should not be shorter than 1us,
 * since elapsed time measurement cannot possibly be any finer in
 * granularity.  In practice, 1ms is probably the best we can hope for
 * in timer resolution, so even if nanosleep suspends for 1us, the
 * gettimeofday resolution may cause a false failure report.
 */

TEST_P(NanoSleepTests, TestNanoSleep) {
  const struct timespec *t_suspend = &GetParam();
  uint64_t slop_ms = 0;
  struct timespec t_remain;
  struct timeval  t_start;
  int             rv;
  struct timeval  t_end;
  struct timeval  t_elapsed;

  printf("%40s: ", "Requesting nanosleep duration");
  PrintTimespec(stdout, t_suspend);
  printf(" seconds\n");
  t_remain = *t_suspend;

  ASSERT_NE(-1, gettimeofday(&t_start, NULL)) << "gettimeofday for start time failed";
  while (-1 == (rv = nanosleep(&t_remain, &t_remain)) &&
         EINTR == errno) {
  }
  ASSERT_NE(-1, rv) << "nanosleep failed, errno = " << errno;
  ASSERT_NE(-1, gettimeofday(&t_end, NULL)) << "gettimeofday for end time failed";

  /*
   * We add a microsecond in case of rounding/synchronization issues
   * between the nanosleep/scheduler clock and the time-of-day clock,
   * where the time-of-day clock doesn't _quite_ get incremented in
   * time even though the entire nanosleep duration had passed.
   * (We've seen this occur on the mac.)
   */

  t_elapsed.tv_sec = t_end.tv_sec - t_start.tv_sec;
  t_elapsed.tv_usec = t_end.tv_usec - t_start.tv_usec + 1;

  NormalizeTimeval(&t_elapsed);
  printf("%40s: ", "Actual nanosleep duration");
  PrintTimeval(stdout, &t_elapsed);
  printf(" seconds\n");

  /*
   * On WinXP, Sleep(num_ms) sometimes -- though rarely -- return
   * earlier than it is supposed to.  This may be due to gettimeofday
   * issues when running on VMs, rather than actual insomnia.  In any
   * case, We permit adding in some slop here to the elapsed time so
   * that we can ignore the sporadic random test failures that would
   * occur.
   */
  t_elapsed.tv_usec += slop_ms * MICROS_PER_MILLI;
  NormalizeTimeval(&t_elapsed);
  printf("%40s: ", "Slop adjusted duration");
  PrintTimeval(stdout, &t_elapsed);
  printf(" seconds\n");

  ASSERT_FALSE(t_elapsed.tv_sec < t_suspend->tv_sec ||
      (t_elapsed.tv_sec == t_suspend->tv_sec &&
       (NANOS_PER_MICRO * t_elapsed.tv_usec < t_suspend->tv_nsec)))
      << "Elapsed time too short!";
}


static struct timespec  t_suspend[] = {
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
                        NanoSleepTests,
                        ::testing::ValuesIn(t_suspend));
