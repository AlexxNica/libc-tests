/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl test for super simple program not using newlib
 */

#include <stdarg.h>
#include "gtest/gtest.h"

namespace {

class StdArgsTests : public ::testing::Test {
 protected:

  StdArgsTests() {
    // You can do set-up work for each test here.
  }

  ~StdArgsTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



/* globals are intentional to confuse optimizer */
int errors = 55;
int count = 0;
char buffer[16];

void dump(const char *fmt, ...) {
  int i;

  va_list ap;
  va_start(ap, fmt);

  count = 0;

  for (i = 0; fmt[i]; ++i) {
    if (fmt[i] == 'L') {
      long l = va_arg(ap, long);
      count++;
      EXPECT_EQ(l, count);
    } else if (fmt[i] == 'Q') {
      long long ll = (long long) va_arg(ap, long long);
      count++;
      EXPECT_EQ(((long)((ll >> 32) & 0xffffffffL)), count);
      count++;
      EXPECT_EQ(((long)(ll & 0xffffffffL)), count);
    } else {
      // Bad format letter!
      EXPECT_TRUE(false) << "Bad format letter: " << fmt[i];
    }
  }
  va_end(ap);
}


TEST_F(StdArgsTests, TestVaargs) {
  dump("QQQL",
       0x0000000100000002LL,
       0x0000000300000004LL,
       0x0000000500000006LL,
       0x00000007L);

  dump("QQLQ",
       0x0000000100000002LL,
       0x0000000300000004LL,
       0x00000005L,
       0x0000000600000007LL);

  dump("QLQQ",
       0x0000000100000002LL,
       0x00000003L,
       0x0000000400000005LL,
       0x0000000600000007LL);

  dump("LQQQ",
       0x00000001L,
       0x0000000200000003LL,
       0x0000000400000005LL,
       0x0000000600000007LL);

  dump("QQQ",
       0x0000000100000002LL,
       0x0000000300000004LL,
       0x0000000500000006LL);

  dump("LLLLLL",
       0x00000001L,
       0x00000002L,
       0x00000003L,
       0x00000004L,
       0x00000005L,
       0x00000006L);
}
