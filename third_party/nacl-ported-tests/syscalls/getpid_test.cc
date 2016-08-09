/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


/*
 * Test for getpid syscall.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gtest/gtest.h"

namespace {

class PidTests : public ::testing::Test {
 protected:
  PidTests() {
    // You can do set-up work for each test here.
  }

  ~PidTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



TEST_F(PidTests, TestGetPid) {
  pid_t pid_one = getpid();
  pid_t pid_two = getpid();

  // check if it's greater than 0.
  EXPECT_EQ(pid_one, pid_two) << "getpid returned different values for same process";
  EXPECT_GT(pid_one, 0) << "Invalid pid returned for the process";
}
