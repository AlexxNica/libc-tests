/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "gtest/gtest.h"

namespace {

class MutexLeakTests : public ::testing::Test {
 protected:
  MutexLeakTests() {
    // You can do set-up work for each test here.
  }

  ~MutexLeakTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



TEST_F(MutexLeakTests, TestMutexLeak) {
 int avail_fd = dup(2);
 close(avail_fd);

 for (int i = 0; i < 10; i++) {
   pthread_mutex_t mutex;
   ASSERT_EQ(0, pthread_mutex_init(&mutex, NULL)) << "Unable to create mutex";
   ASSERT_EQ(0, pthread_mutex_destroy(&mutex)) << "Unable to destroy mutex";
 }

 int next_fd = dup(2);
 ASSERT_EQ(next_fd, avail_fd) << "Leaked descriptors!";

 close(next_fd);

}
