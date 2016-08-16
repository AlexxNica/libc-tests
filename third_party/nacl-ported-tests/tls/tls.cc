/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifdef WITH_PTHREAD
# include <pthread.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include "gtest/gtest.h"

namespace {

class TEST_NAME : public ::testing::Test {
 protected:
  volatile int dummy = 1;
  TEST_NAME() {
    // You can do set-up work for each test here.
  }

  ~TEST_NAME() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



__thread static int tdata1 = 1;
__thread static int tdata2 __attribute__((aligned(0x10))) = 2;
/* We need to test the case when TLS size is not aligned to 16 bytes. */
#ifdef MORE_TDATA
__thread static int tdata_more = 3;
#endif
#ifdef TDATA_LARGE_ALIGN
__thread static int tdata3 __attribute__((aligned(0x1000))) = 4;
#endif
#ifdef WITH_TBSS
__thread static int tbss1;
__thread static int tbss2 __attribute__((aligned(0x10)));
/* If tdata and tbss are aligned separately, we need to check different tbss
   sizes too. */
# ifdef MORE_TBSS
__thread static int tbss_more;
# endif
# ifdef TBSS_LARGE_ALIGN
__thread static int tbss3 __attribute__((aligned(0x1000)));
# endif
#endif

#ifdef WITH_PTHREAD
static void *thread_proc(void *arg) {
  return arg;
}
#endif

static void __attribute__((noinline)) AlignCheck(void *address, int align) {
  EXPECT_EQ((uintptr_t) address % align, 0ul)
      << "Address " << address << " is not aligned to a multiple of "
      << align;
}

TEST_F(TEST_NAME, TESTCASE_NAME) {
#ifdef WITH_PTHREAD
  pthread_t tid;
#endif
  EXPECT_EQ(tdata1, 1);
  EXPECT_EQ(tdata2, 2);
  AlignCheck(&tdata2, 0x10);
#ifdef MORE_TDATA
  EXPECT_EQ(tdata_more, 3);
#endif
#ifdef TDATA_LARGE_ALIGN
  AlignCheck(&tdata3, 0x1000);
#endif

#ifdef WITH_TBSS
  AlignCheck(&tbss2, 0x10);
  ASSERT_EQ(tbss1, 0);
  ASSERT_EQ(tbss2, 0);
  tbss1 = 1;
  tbss2 = 2;
  EXPECT_EQ(tbss1, 1);
  EXPECT_EQ(tbss2, 2);
# ifdef MORE_TBSS
  ASSERT_EQ(tbss_more, 0);
# endif
# ifdef TBSS_LARGE_ALIGN
  ASSERT_EQ(tbss3, 0);
  AlignCheck(&tbss3, 0x1000);
# endif
#endif
#ifdef WITH_PTHREAD
  /* This is dead code but it makes linker use pthread library */
  if (dummy == -1) {
    pthread_create(&tid, NULL, thread_proc, NULL);
  }
#endif
}
