/*
 * Copyright 2009 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Test basic floating point operations
 */

#include <stdio.h>
#include <stdlib.h>

#if 1
#define PRINT_FLOAT(mesg, prec, val)\
 printf(mesg ": %." #prec  "f\n", (double) (val))
#else
#define PRINT_FLOAT(mesg, prec, val)\
  printf(mesg ": %d\n", (int) (val))
#endif

#define PRINT_INT(mesg, val)\
  printf(mesg ": %d\n", (int) (val))

#include "gtest/gtest.h"

namespace {

class FloatTests : public ::testing::Test {
 protected:
  FloatTests() {
    // You can do set-up work for each test here.
  }

  ~FloatTests() override {}

  void SetUp() override {}

  void TearDown() override {}
};

}  // namespace

static std::string float_output[] = {
    R"(val str: 1.0
val int: 1
val flt: 1.000000000
last: 0.000
+: 1.000
-: -1.000
*: 0.000
/: 0.000
)",
    R"(val str: 3.0
val int: 3
val flt: 3.000000000
last: 1.000
+: 4.000
-: -2.000
*: 3.000
/: 0.333
)",
    R"(val str: 0.5
val int: 0
val flt: 0.500000000
last: 3.000
+: 3.500
-: 2.500
*: 1.500
/: 6.000
)",
    R"(val str: 100.0
val int: 100
val flt: 100.000000000
last: 0.500
+: 100.500
-: -99.500
*: 50.000
/: 0.005
)",
    R"(val str: 0.3
val int: 0
val flt: 0.300000012
last: 100.000
+: 100.300
-: 99.700
*: 30.000
/: 333.333
)"};

static std::string double_output[5] = {
    R"(val str: 1.0
val int: 1
val flt: 1.000000000
last: 0.000000000
+: 1.000000000
-: -1.000000000
*: 0.000000000
/: 0.000000000
)",
    R"(val str: 3.0
val int: 3
val flt: 3.000000000
last: 1.000000000
+: 4.000000000
-: -2.000000000
*: 3.000000000
/: 0.333333333
)",
    R"(val str: 0.5
val int: 0
val flt: 0.500000000
last: 3.000000000
+: 3.500000000
-: 2.500000000
*: 1.500000000
/: 6.000000000
)",
    R"(val str: 100.0
val int: 100
val flt: 100.000000000
last: 0.500000000
+: 100.500000000
-: -99.500000000
*: 50.000000000
/: 0.005000000
)",
    R"(val str: 0.3
val int: 0
val flt: 0.300000000
last: 100.000000000
+: 100.300000000
-: 99.700000000
*: 30.000000000
/: 333.333333333
)"};

static const char* input[] = {"1.0", "3.0", "0.5", "100.0", "0.3"};

TEST_F(FloatTests, TestDouble) {
  int i;
  double last = 0.0;
  double x;
  for (i = 0; i < 5; ++i) {
    testing::internal::CaptureStdout();
    printf("val str: %s\n", input[i]);
    x = strtod(input[i], 0);
    PRINT_INT("val int", x);
    PRINT_FLOAT("val flt",9, x);
    PRINT_FLOAT("last", 9, last);
    PRINT_FLOAT("+", 9, last + x);
    PRINT_FLOAT("-", 9, last - x);
    PRINT_FLOAT("*", 9, last * x);
    PRINT_FLOAT("/", 9, last / x);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(double_output[i], output);
    last = x;
  }
}

TEST_F(FloatTests, TestFloat) {
  int i;
  float last = 0.0;
  float x;
  for (i = 0; i < 5; ++i) {
    testing::internal::CaptureStdout();
    printf("val str: %s\n", input[i]);
    x = strtof(input[i], 0);
    PRINT_INT("val int", x);
    PRINT_FLOAT("val flt",9, x);
    PRINT_FLOAT("last", 3, last);
    PRINT_FLOAT("+", 3, last + x);
    PRINT_FLOAT("-", 3, last - x);
    PRINT_FLOAT("*", 3, last * x);
    PRINT_FLOAT("/", 3, last / x);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(float_output[i], output);
    last = x;
  }
}
