/*
 * Copyright 2014 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arpa/inet.h>
#include "gtest/gtest.h"

namespace {
class InetTests : public ::testing::Test {
 protected:

  InetTests() {
    // You can do set-up work for each test here.
  }

  ~InetTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace

TEST_F(InetTests, TestInet) {
  /*
   * System is always little-endian, and network order is big-endian
   * so ntohl(3) and friends are always expected to reverse the
   * byte order.
   */
  uint32_t netlong = 0x01020304;
  uint32_t hostlong = 0x04030201;
  uint32_t netshort = 0x0102;
  uint32_t hostshort = 0x0201;

  ASSERT_EQ(hostlong, ntohl(netlong));
  ASSERT_EQ(netlong, htonl(hostlong));

  ASSERT_EQ(hostshort, ntohs(netshort));
  ASSERT_EQ(netshort, htons(hostshort));
}
