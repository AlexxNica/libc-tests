/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <wchar.h>

#include "gtest/gtest.h"

namespace {

class WcsTollTests : public ::testing::Test {
 protected:

  WcsTollTests() {
    // You can do set-up work for each test here.
  }

  ~WcsTollTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



TEST_F(WcsTollTests, TestWcsToll) {
  const wchar_t *wcs;
  wchar_t *stopwcs;
  long long val;

  wcs = L"1234567890123456789";
  val = wcstoll(wcs, &stopwcs, 10);
  printf("wcstoll = %lld\n", val);
  printf("Stopped scan at `%ls`\n", stopwcs);
  EXPECT_EQ(val, 1234567890123456789LL);
}
