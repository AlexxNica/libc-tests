/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include "gtest/gtest.h"

namespace {

class BigAllocTests : public ::testing::Test {
 protected:

  BigAllocTests() {
    // You can do set-up work for each test here.
  }

  ~BigAllocTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace

TEST_F(BigAllocTests, TestBigAlloc) {
  void *buf = malloc(128 << 20);
  ASSERT_NE(buf, nullptr);
  free(buf);
}
