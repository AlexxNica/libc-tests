/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gtest/gtest.h"

namespace {

class FdOpenTests : public ::testing::Test {
 protected:

  FdOpenTests() {
    // You can do set-up work for each test here.
  }

  ~FdOpenTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace

extern char *testdata_dir;

TEST_F(FdOpenTests, TestFdOpen) {
  const char* test_filename = strcat(testdata_dir, "/fdopen_testdata");
  int fd = open(test_filename, O_RDONLY);
  ASSERT_GT(fd, 0);
  FILE *file = fdopen(fd, "r");
  ASSERT_EQ(fd, fileno(file));
  char buf[100];
  size_t count = fread(buf, 1, sizeof buf, file);
  const char expected[] = "Testing Data!";
  ASSERT_EQ(count, strlen(expected));
  ASSERT_EQ(memcmp(buf, "Testing Data!", count), 0);
  fclose(file);
}
