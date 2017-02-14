/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <dlfcn.h>
#include <threads.h>

#include "gtest/gtest.h"

namespace {

class DISABLED_DlOpenTests : public ::testing::Test {
 protected:

  DISABLED_DlOpenTests() {
    // You can do set-up work for each test here.
  }

  ~DISABLED_DlOpenTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



typedef void (*call_once_t)(once_flag*, void (*func)(void));

/*
 * This union is required only to convert void* to function pointer. ISO C does
 * not provide such a conversion.
 */
typedef union {
  call_once_t pfunc;
  void* pvoid;
} dl_union;

static int test_val;

void do_work(void) {
  test_val = 2;
}

// disable this test for now, since it seems to wedge up instead of failing
TEST_F(DISABLED_DlOpenTests, TestDlOpen) {
  void *handle;
  dl_union f;

  handle = dlopen("libc++.so.2", RTLD_LAZY);
  ASSERT_NE(nullptr, handle);
  dlerror();
  f.pvoid = dlsym(handle, "call_once");
  char *err = dlerror();
  ASSERT_EQ(nullptr, err) << err;
  once_flag flag;
  test_val = 1;
  (*f.pfunc)(&flag, do_work);
  EXPECT_EQ(2, test_val) << "do_work should have assigned it 2.";
  ASSERT_EQ(0, dlclose(handle));
}
