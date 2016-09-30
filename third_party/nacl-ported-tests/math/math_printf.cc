/*
 * Copyright 2009 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"

namespace {

class MathPrintfTests : public ::testing::Test {};

}  // namespace

static std::string expected_output =
    R"(1000
1000
3e8
0x000003e8

1
1
1
0x00000001

0
0
0
0x00000000

)";

const char* input[] = {"1000", "1", "0"};
TEST_F(MathPrintfTests, Test) {
  int i;

  testing::internal::CaptureStdout();
  for (i = 0; i < 3; ++i) {
    long x;
    printf("%s\n", input[i]);

    x = strtol(input[i], 0, 0);
    printf("%ld\n", x);
    printf("%lx\n", x);
    printf("0x%08lx\n", x);
    printf("\n");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(expected_output, output);
}
