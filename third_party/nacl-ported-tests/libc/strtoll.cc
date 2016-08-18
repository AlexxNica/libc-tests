/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gtest/gtest.h"
namespace {

struct test_param {
  const char *str;
  int64_t want;
  int base;
};

class StrTollTests : public ::testing::TestWithParam<struct test_param> {
 protected:

  StrTollTests() {
    // You can do set-up work for each test here.
  }

  ~StrTollTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


TEST_P(StrTollTests, TestStrToll) {
  const struct test_param *params = &GetParam();
  const char *str = params->str;
  int64_t want = params->want;
  int base = params->base;
  int64_t val;
  errno = 0; /* errno is undefined on success unless initialized */
  val = strtoll(str, 0, base);
  EXPECT_EQ(errno, 0) << "strtoll(" << str << ", 0, "
                      << base << "):", strerror(errno);
  ASSERT_EQ(val, want);
}

struct test_param params[] = {
    { "-5", -5, 10},
    { "7FFFFFFFFFFFFFFF", 0x7FFFFFFFFFFFFFFFLL, 16}
};

INSTANTIATE_TEST_CASE_P(StrToll,
                        StrTollTests,
                        ::testing::ValuesIn(params));

