/*
 * Copyright (c) 2014 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gtest/gtest.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

// Macro to generate variable
#define VAR0(x) x = get_next()
#define VAR1(x)                                                                \
  VAR0(x##0)                                                                   \
  , VAR0(x##1), VAR0(x##2), VAR0(x##3), VAR0(x##4), VAR0(x##5), VAR0(x##6),    \
      VAR0(x##7), VAR0(x##8), VAR0(x##9)

#define COMP(var, val) check_equals(var, val);
#define CHECK_VAL(x, i, s)                                                     \
  COMP(x##i##0, s + i##0);                                                     \
  COMP(x##i##1, s + i##1);                                                     \
  COMP(x##i##2, s + i##2);                                                     \
  COMP(x##i##3, s + i##3);                                                     \
  COMP(x##i##4, s + i##4);                                                     \
  COMP(x##i##5, s + i##5);                                                     \
  COMP(x##i##6, s + i##6);                                                     \
  COMP(x##i##7, s + i##7);                                                     \
  COMP(x##i##8, s + i##8);                                                     \
  COMP(x##i##9, s + i##9)

namespace {

class LongJmpStackSlotsTests : public ::testing::Test {
protected:
  LongJmpStackSlotsTests() {
    // You can do set-up work for each test here.
  }

  ~LongJmpStackSlotsTests() override {}

  void SetUp() override {}

  void TearDown() override {}
};

} // namespace

static int g_counter = 9;

__attribute__((noinline)) int get_next(void) { return ++g_counter; }

__attribute__((noinline)) void check_equals(int actual, int expected) {
  ASSERT_EQ(actual, expected);
}

/*
 * This tests the reuse of stack slots in cases where use of
 * setjmp() should disable that.
 *
 * this test must be compiled with optimisation, in order
 * to run mem2reg and use spill slots.  However, get_next() and
 * check_equals() must not be inlined.
 */

TEST_F(LongJmpStackSlotsTests, TestLongJmpStackSlots) {
  /*
   * Keep enough variables live across the setjmp() call that they don't
   * all fit in registers, so that the compiler spills some of them to
   * spill slots on the stack.
   *
   * There need to be >31 variables here to test this on aarch64.
   */
  int VAR1(a1);
  int VAR1(a2);
  int VAR1(a3);
  int VAR1(a4);
  jmp_buf buf;
  if (setjmp(buf)) {
    CHECK_VAL(a, 1, 0);
    CHECK_VAL(a, 2, 0);
    CHECK_VAL(a, 3, 0);
    CHECK_VAL(a, 4, 0);
  } else {
    /*
     * Again, keep enough variables live that some of them will need spill
     * slots.  A correct compiler will realise that a1...aN are still live
     * (via setjmp()+longjmp()), and so not reuse the earlier spill slots.
     * An incorrect compiler will think that a1..aN are dead here and wrongly
     * reuse the earlier spill slots.
     */
    int VAR1(b1);
    int VAR1(b2);
    int VAR1(b3);
    int VAR1(b4);
    int start = 40;
    CHECK_VAL(b, 1, start);
    CHECK_VAL(b, 2, start);
    CHECK_VAL(b, 3, start);
    CHECK_VAL(b, 4, start);
    longjmp(buf, 1);
  }
}
