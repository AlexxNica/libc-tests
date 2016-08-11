/*
 * Copyright 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gtest/gtest.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

namespace {

class LongJmpTests : public ::testing::Test {
protected:
  LongJmpTests() {
    // You can do set-up work for each test here.
  }

  ~LongJmpTests() override {}

  void SetUp() override {}

  void TearDown() override {}
};

} // namespace

int func1(jmp_buf* env, int x) {
  if (x == 0)
    longjmp(*env, 1);
  return 1;
}

int func2(jmp_buf* env, int x) {
  if (x == 0)
    longjmp(*env, 2);
  return func1(env, x - 1) + 1;
}

int func3(jmp_buf* env, int x) {
  if (x == 0)
    longjmp(*env, 3);
  return func2(env, x - 1) + 1;
}

TEST_F(LongJmpTests, TestLongJmp) {
  jmp_buf env;
  int value;

  value = setjmp(env);
  if (value == 0) {
    func3(&env, 0);
    ASSERT_TRUE(false) << "Control should not reach here";
  } else {
    ASSERT_EQ(3, value);
  }

  value = setjmp(env);
  if (value == 0) {
    func3(&env, 1);
    ASSERT_TRUE(false) << "Control should not reach here";
  } else {
    ASSERT_EQ(2, value);
  }

  value = setjmp(env);
  if (value == 0) {
    func3(&env, 2);
    ASSERT_TRUE(false) << "Control should not reach here";
  } else {
    ASSERT_EQ(1, value);
  }

  value = setjmp(env);
  ASSERT_EQ(0, value);
  ASSERT_EQ(func3(&env, 3), 3);
}
