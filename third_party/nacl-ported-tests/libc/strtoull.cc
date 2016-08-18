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
  uint64_t want;
  int base;
};

class StrToullTests : public ::testing::TestWithParam<struct test_param> {
 protected:

  StrToullTests() {
    // You can do set-up work for each test here.
  }

  ~StrToullTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


TEST_P(StrToullTests, TestStrToull) {
  const struct test_param *params = &GetParam();
  const char *str = params->str;
  uint64_t want = params->want;
  int base = params->base;
  uint64_t val;
  errno = 0; /* errno is undefined on success unless initialized */
  val = strtoull(str, 0, base);
  EXPECT_EQ(errno, 0) << "strtoull(" << str << ", 0, "
                      << base << "):", strerror(errno);
  ASSERT_EQ(val, want);
}

struct test_param params[] = {
    { "5", 5, 10 },
    { "FFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFFLL, 16 }
};

INSTANTIATE_TEST_CASE_P(StrToull,
                        StrToullTests,
                        ::testing::ValuesIn(params));

