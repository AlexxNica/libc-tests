/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "gtest/gtest.h"

#define DEFAULT_CONDVAR_TIMEOUT_MS (100)
#define DEFAULT_CPU_THRESHOLD_MS   (10 + (70-10)/2)

namespace {

class CondTimedWaitTests : public ::testing::Test {
 protected:
  CondTimedWaitTests() {
    // You can do set-up work for each test here.
  }

  ~CondTimedWaitTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


TEST_F(CondTimedWaitTests, TestCondTimedWait) {
  unsigned long t_timeout_msec = DEFAULT_CONDVAR_TIMEOUT_MS;
  pthread_mutex_t mu;
  pthread_cond_t cv;
  struct timespec ts;
  clock_t t_start;
  clock_t t_end;
  double t_cpu_used_ms;
  unsigned long t_cpu_threshold_ms = DEFAULT_CPU_THRESHOLD_MS;
  ASSERT_EQ(0, pthread_mutex_init(&mu,
                                    (pthread_mutexattr_t const *) NULL));

  ASSERT_EQ(0, pthread_cond_init(&cv,
                                   (pthread_condattr_t const *) NULL));

  ASSERT_EQ(0, pthread_mutex_lock(&mu));

  ASSERT_EQ(0, clock_gettime(CLOCK_REALTIME, &ts));
  ts.tv_sec += t_timeout_msec / 1000;
  ts.tv_nsec += (t_timeout_msec % 1000) * 1000 * 1000;
  if (ts.tv_nsec > 1000000000) {
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000;
  }
  t_start = clock();
  ASSERT_EQ(ETIMEDOUT, pthread_cond_timedwait(&cv, &mu, &ts));
  t_end = clock();
  t_cpu_used_ms = 1000.0 * ((double) (t_end - t_start)) / CLOCKS_PER_SEC;
  EXPECT_LE(t_cpu_used_ms, t_cpu_threshold_ms) << "Excess CPU usuage";
}
