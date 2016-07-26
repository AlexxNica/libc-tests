/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Test basic floating point operations
 */
#include "gtest/gtest.h"
#include <math.h>

#define NUM_ARRAY_ELEMENTS 3

namespace {

class MiscMathTests : public ::testing::Test {
 protected:
  volatile int dummy;
  float Numbers1[NUM_ARRAY_ELEMENTS] = {0.5, 0.3, 0.2};

  MiscMathTests() {
    // You can do set-up work for each test here.
  }

  ~MiscMathTests() override {
  }


  void SetUp() override {
    dummy = 10;
  }

  void TearDown() override {
  }
};

} //namespace



TEST_F(MiscMathTests, MiscMath) {
  int i;
  float sum;

  /* This will not happen but helps confusing the optimizer */
  if (dummy > 1000) {
    for (i = 0; i < NUM_ARRAY_ELEMENTS; ++i) {
      Numbers1[i] = (float) dummy;
    }
  }

    /* actual computation */
  sum = 0.0;
  for (i = 0; i < NUM_ARRAY_ELEMENTS; ++i) {
    sum += Numbers1[i];
  }

  ASSERT_LT(fabs (sum - 1.), 0.001);
}
