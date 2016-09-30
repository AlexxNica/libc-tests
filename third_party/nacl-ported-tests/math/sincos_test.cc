/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"

void sincos(double a, double *s, double *c);
void sincosf(float a, float *s, float *c);

template <typename T>
std::vector<T> generate_params() {
  std::vector<T> input;
  const double pi = 3.14159265;
  const double fourpi = 4.0 * pi;
  double a;

  for (a = -fourpi; a < fourpi; a += pi / 8.0) {
    input.push_back(a);
  }
  /* seed drand48() generator to make it deterministic. */
  srand48(12345678);
  if (std::is_same<T, double>::value) {
    input.push_back(fourpi * (drand48() - 0.5));
  } else {
    input.push_back(drand48());
  }
  return input;
}

namespace {

class SinCosDoubleTests : public ::testing::TestWithParam<double> {};
class SinCosFloatTests : public ::testing::TestWithParam<float> {};

}  // namespace

TEST_P(SinCosDoubleTests, TestSinCos) {
  double a = GetParam();
  const double maxerr = 0.000000000001;
  double sincos_sin, sincos_cos, sin_sin, cos_cos;

  sincos(a, &sincos_sin, &sincos_cos);
  sin_sin = sin(a);
  cos_cos = cos(a);
  if (fabs(sincos_sin - sin_sin) > maxerr ||
      fabs(sincos_cos - cos_cos) > maxerr) {
    char fail_msg[256];
    sprintf(fail_msg,
            "sincosf(%12.12f) outside tolerance: sin:%12.12f, cos:%12.12f\n", a,
            sincos_sin - sin_sin, sincos_cos - cos_cos);
    FAIL() << fail_msg;
  }
}

INSTANTIATE_TEST_CASE_P(SinCosD,
                        SinCosDoubleTests,
                        ::testing::ValuesIn(generate_params<double>()));

TEST_P(SinCosFloatTests, TestSinCos) {
  float a = GetParam();
  const float maxerr = 0.000000000001;
  float sincos_sin, sincos_cos, sin_sin, cos_cos;

  sincosf(a, &sincos_sin, &sincos_cos);
  sin_sin = sinf(a);
  cos_cos = cosf(a);
  if (fabsf(sincos_sin - sin_sin) > maxerr ||
      fabsf(sincos_cos - cos_cos) > maxerr) {
    char fail_msg[256];
    sprintf(fail_msg,
            "sincosf(%12.12f) outside tolerance: sin:%12.12f, cos:%12.12f\n", a,
            sincos_sin - sin_sin, sincos_cos - cos_cos);
    FAIL() << fail_msg;
}
}

INSTANTIATE_TEST_CASE_P(SinCosF,
                        SinCosFloatTests,
                        ::testing::ValuesIn(generate_params<float>()));
