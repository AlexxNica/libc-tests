/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "gtest/gtest.h"

namespace {

class ManyThreadsSeqTests : public ::testing::Test {
 protected:
  ManyThreadsSeqTests() {
    // You can do set-up work for each test here.
  }

  ~ManyThreadsSeqTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



/*
 * This test checks that we can successfully create and join threads
 * repeatedly, without any leaks that would cause this to eventually
 * fail.
 *
 */
const int kIterations = 10000;

void *thread_func(void *unused_arg) {
  return NULL;
}

TEST_F(ManyThreadsSeqTests, TestManyThreadsSeq) {
  int rc;
  int index;
  for (index = 0; index < kIterations; index++) {
    pthread_t tid;
    rc = pthread_create(&tid, NULL, thread_func, NULL);
    ASSERT_EQ(rc, 0);
    rc = pthread_join(tid, NULL);
    ASSERT_EQ(rc, 0);
  }
}
