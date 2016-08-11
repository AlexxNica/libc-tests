/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gtest/gtest.h"
#include <setjmp.h>
#include <stdio.h>

namespace {

class SetLongJmpTests : public ::testing::TestWithParam<int> {
protected:
  SetLongJmpTests() {
    // You can do set-up work for each test here.
  }

  ~SetLongJmpTests() override {}

  void SetUp() override {}

  void TearDown() override {}
};

} // namespace

static jmp_buf buf;

TEST_P(SetLongJmpTests, TestSetLongJmp) {
  int longjmp_arg = GetParam();
  volatile int result = -1;
  int setjmp_ret = -1;

  setjmp_ret = setjmp(buf);
  if (!setjmp_ret) {
    /* Check that setjmp() doesn't return 0 multiple times */
    ASSERT_EQ(result, -1);

    result = 55;
    longjmp(buf, longjmp_arg);
    ASSERT_TRUE(false) << "this statement should not be reached";
  } else {
    int expected_ret = longjmp_arg != 0 ? longjmp_arg : 1;
    ASSERT_EQ(setjmp_ret, expected_ret);
  }
}

INSTANTIATE_TEST_CASE_P(SetLongJmp, SetLongJmpTests,
                        ::testing::Values(1, 0, -1));
