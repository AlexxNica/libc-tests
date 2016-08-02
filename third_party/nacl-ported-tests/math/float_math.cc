/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Test parts of math.h and floating point ops for compliance against ieee754.
 */

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include "gtest/gtest.h"

#define CHECK_NAN(_x_) EXPECT_TRUE(isnan(_x_))

#define CHECK_INF(a) EXPECT_TRUE(isinf(a))

namespace {

class FloatMathTests : public ::testing::Test {
 protected:

  FloatMathTests() {
  }

  ~FloatMathTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



/************************************************************/

TEST_F(FloatMathTests, TestConstants) {
  /* Attempt to prevent constant folding */
  volatile double x = 1.0;
  volatile double y = 0.0;
  CHECK_NAN(NAN);
  printf("Print out of NaN: %f\n", NAN);
  CHECK_INF(INFINITY);
  printf("Print out of Infinity: %f\n", INFINITY);
  CHECK_INF(x/y);
}

TEST_F(FloatMathTests, TestCompares) {
  /* Attempt to prevent constant folding */
  volatile double x;
  volatile double y;

  printf("Comparing float constants\n");
  x = NAN;
  ASSERT_TRUE(x != x);
  ASSERT_TRUE(isunordered(x, x));
  CHECK_NAN(x + 3.0f);
  CHECK_NAN(x + x);
  CHECK_NAN(x - x);
  CHECK_NAN(x / x);
  CHECK_NAN(0.0 / x);
  CHECK_NAN(0.0 * x);

  ASSERT_FALSE(x == x);

  x = INFINITY;
  ASSERT_TRUE(x == x);
  ASSERT_FALSE(x == -x);
  ASSERT_TRUE(-x == -x);
  ASSERT_TRUE(x + 100.0 == x);
  ASSERT_TRUE(x - 100.0 == x);
  ASSERT_TRUE(-x - 100.0 == -x);
  ASSERT_TRUE(-x + 100.0 == -x);
  ASSERT_TRUE(-x < x);
  ASSERT_FALSE(-x > x);

  y = 0.0;
  CHECK_NAN(y * x);
  CHECK_NAN(y / y);
  CHECK_NAN(x / x);
  CHECK_NAN(x - x);
  y = NAN;
  CHECK_NAN(x * y);

  x = INFINITY;
  CHECK_INF(x + x);
  x = 1.0; y = 0.0;
  CHECK_INF(x / y);

  x = INFINITY;
  ASSERT_FALSE(isfinite(x));
  x = NAN;
  ASSERT_FALSE(isfinite(x));
}

/* Test non-NaN-resulting library calls. */
TEST_F(FloatMathTests, TestDefined) {
  /*
   * Attempt to prevent constant folding and optimization of library
   * function bodies (when statically linked).
   */
  volatile double x;
  volatile double y;
  volatile double z;

  printf("Checking lib calls that take NaN, etc, but return non-NaN.\n");
  x = 0.0; y = 1.0;
  EXPECT_EQ(pow(x, x), y);
  z = NAN;
  EXPECT_EQ(pow(z, x), y);
  z = INFINITY;
  EXPECT_EQ(pow(z, x), y);

  CHECK_INF(sqrt(z));
  x = -0.0;
  EXPECT_EQ(sqrt(x), x);
  x = INFINITY; y = 2.0;
  CHECK_INF(pow(x, y));
  x = 0.0; y = -INFINITY;
  ASSERT_TRUE(log(x) == y);
}

/* Test NaN-resulting library calls. */
TEST_F(FloatMathTests, TestErrs) {
  /*
   * Attempt to prevent constant folding and optimization of library
   * function bodies (when statically linked).
   */
  volatile double x;
  volatile double y;

  printf("Checking well-defined library errors\n");
  x = -3.0; y = 4.4;
  CHECK_NAN(pow(x, y));
  CHECK_NAN(log(x));
  x = -0.001;
  CHECK_NAN(sqrt(x));
  x = 1.0001;
  CHECK_NAN(asin(x));
  x = INFINITY;
  CHECK_NAN(sin(x));
  CHECK_NAN(cos(x));
  x = 0.999;
  CHECK_NAN(acosh(x));
  x = 3.3; y = 0.0;
  CHECK_NAN(remainder(x, y));
  y = INFINITY;
  CHECK_NAN(remainder(y, x));
}

/* Check exceptions communicated by the "old" errno mechanism */
#define CHECK_TRIPPED_ERRNO(expr)                             \
   errno = 0; (void)expr; EXPECT_EQ(errno, 0)

TEST_F(FloatMathTests, TestException) {
  /*
   * Attempt to prevent constant folding and optimization of library
   * function bodies (when statically linked).
   */
  volatile double x;
  volatile double y;

  printf("Checking that exceptional lib calls set errno\n");
  x = -3.0; y = 4.4;
  CHECK_TRIPPED_ERRNO(pow(x, y));
  CHECK_TRIPPED_ERRNO(log(x));
  x = -0.001;
  CHECK_TRIPPED_ERRNO(sqrt(x));
  x = 1.0001;
  CHECK_TRIPPED_ERRNO(asin(x));

  x = INFINITY;
  CHECK_TRIPPED_ERRNO(sin(x));
  CHECK_TRIPPED_ERRNO(cos(x));

  x = 0.999;
  CHECK_TRIPPED_ERRNO(acosh(x));
  x = 3.3; y = 0.0;
  CHECK_TRIPPED_ERRNO(remainder(x, y));

  x = INFINITY; y = 3.3;
  CHECK_TRIPPED_ERRNO(remainder(x, y));
}
