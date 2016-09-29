/*
 * Copyright (c) 2013 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gtest/gtest.h"
#include <cmath>
#include <cstdio>

using std::pow;
using std::printf;

namespace {

class CxxPowTests : public ::testing::Test {
protected:
  CxxPowTests() {
    // You can do set-up work for each test here.
  }

  ~CxxPowTests() override {}

  void SetUp() override {}

  void TearDown() override {}
};

} // namespace

/* Picking a base that can be represented exactly in FP, even when squared */
static volatile float fnum_base = 4.125;
static volatile float fnum_two = 2.0;
static volatile int inum_two = 2;

static volatile double dnum_base = 16.5;
static volatile double dnum_two = 2.0;

TEST_F(CxxPowTests, TestPow) {
  EXPECT_EQ(17.015625, pow(fnum_base, fnum_two));
  EXPECT_EQ(17.015625, pow(fnum_base, inum_two));
  char output[10];
  /* Only 2 digits after the dot are needed, but avoid appending 0s */
  sprintf(output, "%.2f", pow(dnum_base, dnum_two));
  EXPECT_STREQ("272.25", output);
  sprintf(output, "%.2f", pow(dnum_base, inum_two));
  EXPECT_STREQ("272.25", output);
}
