/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gtest/gtest.h"
#include <stdio.h>
#include <string>

namespace {

class RedirTests : public ::testing::Test {
 protected:

  RedirTests() {
    // You can do set-up work for each test here.
  }

  ~RedirTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


TEST_F(RedirTests, TestRedir) {
  const char* stdout_str = "REALOUTPUT: Hello standard output\n";
  const char*  stderr_str = "REALOUTPUT: Hello standard error!\n";

  testing::internal::CaptureStdout();
  printf("%s", stdout_str);
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(output.c_str(), stdout_str);

  testing::internal::CaptureStderr();
  fprintf(stderr, "%s", stderr_str);
  output = testing::internal::GetCapturedStderr();
  EXPECT_STREQ(output.c_str(), stderr_str);
}
