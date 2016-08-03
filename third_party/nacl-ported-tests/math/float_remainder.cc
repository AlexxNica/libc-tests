/*
 * Copyright (c) 2014 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include "gtest/gtest.h"

#define CHECK_ERRNO(expected)                                                  \
  do {                                                                         \
    EXPECT_EQ(expected, errno);                                                \
    /* Reset errno to something predictable. */                                \
    errno = 0;                                                                 \
  } while (0)

#define CHECK_NAN(err, numer, denom)                                           \
  do {                                                                         \
    double res = fmod(numer, denom);                                           \
    CHECK_ERRNO(err);                                                          \
    EXPECT_TRUE(isnan(res));                                                   \
  } while (0)

#define CHECK_NANF(err, numer, denom)                                          \
  do {                                                                         \
    float res = fmodf(numer, denom);                                           \
    CHECK_ERRNO(err);                                                          \
    EXPECT_TRUE(isnan(res));                                                   \
  } while (0)

const double kTolerance = DBL_EPSILON;
const double kToleranceF = FLT_EPSILON * 2;

#define CHECK_EQ(expect, numer, denom)                                         \
  do {                                                                         \
    double res = fmod(numer, denom);                                           \
    CHECK_ERRNO(0);                                                            \
    /* The tolerance check may not work for SUBNORMAL, NaN, etc., so check */  \
    EXPECT_TRUE(fpclassify(res) == FP_NORMAL || fpclassify(res) == FP_ZERO);   \
    EXPECT_LE(fabs(expect - res), kTolerance);                                 \
  } while (0)

#define CHECK_EQF(expect, numer, denom)                                        \
  do {                                                                         \
    float res = fmodf(numer, denom);                                           \
    CHECK_ERRNO(0);                                                            \
    /* The tolerance check may not work for SUBNORMAL, NaN, etc., so check */  \
    EXPECT_TRUE(fpclassify(res) == FP_NORMAL || fpclassify(res) == FP_ZERO);   \
    EXPECT_LE(fabsf(expect - res), kToleranceF);                               \
  } while (0)

namespace {

class FloatRemainderTests : public ::testing::Test {
 protected:

  FloatRemainderTests() {
  }

  ~FloatRemainderTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


TEST_F(FloatRemainderTests, TestRemainder) {
  /* Set up some volatile constants to block the optimizer. */
  volatile double zero = 0.0;
  volatile double nan = NAN;
  volatile double two = 2.0;
  volatile double onesix = 1.6;
  volatile double infinity = INFINITY;
  int expected_errno_infinity = 0;
  int expected_errno_zerodiv = 0;

  /* Initialize errno to something predictable. */
  errno = 0;

  /* If x or y is a NaN, a NaN is returned. */
  CHECK_NAN(0, nan, two);
  CHECK_NANF(0, (float)nan, (float)two);

  CHECK_NAN(0, two, nan);
  CHECK_NANF(0, (float)two, (float)nan);

  CHECK_NAN(0, -onesix, nan);
  CHECK_NANF(0, (float)-onesix, (float)nan);

  CHECK_NAN(0, nan, nan);
  CHECK_NANF(0, (float)nan, (float)nan);

  CHECK_NAN(0, nan, infinity);
  CHECK_NANF(0, (float)nan, (float)infinity);

  /* If x is infinity, a NaN is returned and errno is
   * expected_errno_infinity (see note about BUGS).
   */
  CHECK_NAN(expected_errno_infinity, infinity, two);
  CHECK_NANF(expected_errno_infinity, (float)infinity, (float)two);

  CHECK_NAN(expected_errno_infinity, -infinity, two);
  CHECK_NANF(expected_errno_infinity, (float)-infinity, (float)two);

  /* If y is zero, a NaN is returned and errno is expected_errno_zerodiv. */
  CHECK_NAN(expected_errno_zerodiv, two, zero);
  CHECK_NANF(expected_errno_zerodiv, (float)two, (float)zero);

  CHECK_NAN(expected_errno_zerodiv, two, -zero);
  CHECK_NANF(expected_errno_zerodiv, (float)two, (float)-zero);

  CHECK_NAN(expected_errno_zerodiv, infinity, zero);
  CHECK_NANF(expected_errno_zerodiv, (float)infinity, (float)zero);

  CHECK_NAN(expected_errno_zerodiv, infinity, -zero);
  CHECK_NANF(expected_errno_zerodiv, (float)infinity, (float)-zero);

  /* If x is +0 (-0), and y is not zero, a +0 (-0) is returned. */
  CHECK_EQ(zero, zero, two);
  CHECK_EQF((float)zero, (float)zero, (float)two);
  CHECK_EQ(zero, zero, -two);
  CHECK_EQF((float)zero, (float)zero, (float)-two);

  CHECK_EQ(-zero, -zero, two);
  CHECK_EQF((float)-zero, (float)-zero, (float)two);

  CHECK_EQ(-zero, -zero, -two);
  CHECK_EQF((float)-zero, (float)-zero, (float)-two);

  /*
   * On success... the returned value has the same sign as x and a magnitude
   * less than the magnitude of y.
   */
  CHECK_EQ(1.2, 5.2, two);
  CHECK_EQF(1.2f, 5.2f, (float)two);

  CHECK_EQ(-0.6, -0.6, two);
  CHECK_EQF(-0.6f, -0.6f, (float)two);

  CHECK_EQ(-0.6, -0.6, -two);
  CHECK_EQF(-0.6f, -0.6f, (float)-two);

  CHECK_EQ(zero, 6.4, onesix);
  CHECK_EQF((float)zero, 6.4, (float)onesix);

  CHECK_EQ(1.0, 5.0, two);
  CHECK_EQF(1.0f, 5.0, (float)two);
  CHECK_EQ(-1.0, -5.0, two);
  CHECK_EQF(-1.0f, -5.0, (float)two);

  CHECK_EQ(zero, 100.0, two);
  CHECK_EQF((float)zero, 100.0, (float)two);
  CHECK_EQ(-zero, -100.0, two);
  CHECK_EQF((float)-zero, -100.0, (float)two);

  /* If the numerator is finite and the denominator is an infinity, the
   * result is the numerator.
   */
  CHECK_EQ(5.2, 5.2, infinity);
  CHECK_EQF(5.2f, 5.2f, (float)infinity);

  CHECK_EQ(5.2, 5.2, -infinity);
  CHECK_EQF(5.2f, 5.2f, (float)-infinity);

  CHECK_EQ(-5.2, -5.2, infinity);
  CHECK_EQF(-5.2f, -5.2f, (float)infinity);
}
