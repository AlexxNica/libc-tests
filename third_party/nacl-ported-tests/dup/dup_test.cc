/*
 * Copyright 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * A simple test to ensure that dup and dup2 are working.
 */

#include <stdio.h>
#include <unistd.h>
#include "gtest/gtest.h"

namespace {

class DupTests : public ::testing::Test {
 protected:

  DupTests() {
    // You can do set-up work for each test here.
  }

  ~DupTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace




TEST_F(DupTests, TestDup) {
  FILE  *alt;
  // char  buf[1024];
  int   rv;

  fflush(NULL);
  EXPECT_NE(-1, (rv = dup(1)));
  alt = fdopen(rv, "w");
  fprintf(alt, "Hello world (dup)\n");
  fflush(NULL);
  fclose(alt);
  EXPECT_EQ(3, (rv = dup2(0, 3)));

  // TODO : support gets in tests
  // alt = fdopen(3, "r");
  // printf("%s", fgets(buf, sizeof buf, alt));
  // fclose(alt);

  EXPECT_EQ(3, dup2(1, 3));
  alt = fdopen(3, "w");
  fprintf(alt, "Good bye cruel world! dup2(1, 3)\n");
  fclose(alt);
  EXPECT_EQ(20, dup2(1, 20));
  alt = fdopen(20, "w");
  fprintf(alt, "Good bye cruel world! dup2(1, 20)\n");
  fclose(alt);
}
