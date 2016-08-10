/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <unistd.h>

#include "gtest/gtest.h"

namespace {

class SleepTests : public ::testing::Test {
 protected:

  SleepTests() {
    // You can do set-up work for each test here.
  }

  ~SleepTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace

TEST_F(SleepTests, TestSleep) {
  clock_t start, end;
  start = time(NULL);
  unsigned int unslept = sleep(1);
  end = time(NULL);
  time_t elapsed_time = (end - start);
  EXPECT_EQ(elapsed_time, 1); // Should have slept for 1 sec
  EXPECT_EQ(unslept, 0u);
}
