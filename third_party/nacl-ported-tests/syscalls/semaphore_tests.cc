/*
 * Copyright 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <semaphore.h>

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <cstdlib>
#include <limits>
#include "gtest/gtest.h"

namespace {

class SemaphoreTests : public ::testing::Test {
 protected:

  SemaphoreTests() {
    // You can do set-up work for each test here.
  }

  ~SemaphoreTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace



// Test error conditions of sem_init and return the number of failed checks.
//
// According to the man page on Linux:
// ===================================
// RETURN VALUE
//        sem_init() returns 0 on success; on error, -1 is returned, and errno
//        is set to indicate the error.
//
// ERRORS
//        EINVAL value exceeds SEM_VALUE_MAX.
//
//        ENOSYS pshared is non-zero, but the system does  not  support
//               process-shared semaphores (see sem_overview(7)).
// ===================================
// pshared is not supported in fuchsia, so a non-zero pshared
// value should yield an error (EINVAL).
TEST_F(SemaphoreTests, TestSemInitErrors) {
  // First, make sure that it is possible to exceed SEM_VALUE_MAX
  // for this test, otherwise we can't cause this failure mode.
  EXPECT_LT((const unsigned int)SEM_VALUE_MAX, std::numeric_limits<unsigned int>::max());

  sem_t my_semaphore;

  // Create a value just beyond SEM_VALUE_MAX, try to initialize the semaphore.
  const unsigned int sem_max_plus_1 = (unsigned) SEM_VALUE_MAX + 1;

  // sem_init should return -1 and errno should equal EINVAL
  EXPECT_EQ(-1, sem_init(&my_semaphore, 0, sem_max_plus_1));
  EXPECT_EQ(EINVAL, errno);

  // Try with the largest possible unsigned int.
  EXPECT_EQ(-1, sem_init(&my_semaphore,
                        0,
                        std::numeric_limits<unsigned int>::max()));
  EXPECT_EQ(EINVAL, errno);
}

// Test error conditions of sem_post and return the number of failed checks.
//
// According to the man page on Linux:
// ===================================
// RETURN VALUE
//        sem_post() returns 0 on success; on error, the value of the semaphore
//        is left unchanged, -1 is returned, and errno is set to indicate the
//        error.
//
//  ERRORS
//         EINVAL sem is not a valid semaphore.
//
//         EOVERFLOW
//                The maximum allowable value for a semaphore would be exceeded.
// ===================================
TEST_F(SemaphoreTests, TestSemPostErrors) {
  // Initialize a semaphore with the max value, and try to post to it.
  sem_t my_semaphore;
  EXPECT_EQ(0, sem_init(&my_semaphore, 0, SEM_VALUE_MAX));
  EXPECT_EQ(-1, sem_post(&my_semaphore));
  EXPECT_EQ(EOVERFLOW, errno);
  EXPECT_EQ(0, sem_destroy(&my_semaphore));
}

// The real type of the void* argument to PostThreadFunc.  See PostThreadFunc
// for more information.
struct PostThreadArg {
  // The semaphore to which PostThreadFunc will post.
  sem_t* semaphore;
  // An amount of time to sleep before each post (in microseconds).
  unsigned int sleep_microseconds;
  // The number of times to post before exiting the function.
  unsigned int iterations;
};

// Post to the given semaphore some number of times, with a sleep before each
// post.  poster_thread_arg must be of type PosterThreadArg.  Returns NULL.
void* PostThreadFunc(void* poster_thread_arg) {
  PostThreadArg* pta = static_cast<PostThreadArg*>(poster_thread_arg);
  for (unsigned int i = 0; i < pta->iterations; ++i) {
    usleep(pta->sleep_microseconds);
    sem_post(pta->semaphore);
  }
  return NULL;
}

TEST_F(SemaphoreTests, TestSemNormalOperation) {

  // Test 1 thread posting to 1 semaphore.
  sem_t my_semaphore;
  EXPECT_EQ(0, sem_init(&my_semaphore, 0, 0));
  PostThreadArg pta = { &my_semaphore, /* semaphore */
                        500000u, /* sleep_microseconds */
                        1 /* iterations */ };
  pthread_t my_thread;
  EXPECT_EQ(0, pthread_create(&my_thread, 0, &PostThreadFunc, &pta));
  EXPECT_EQ(0, sem_wait(&my_semaphore));
  EXPECT_EQ(0, pthread_join(my_thread, 0));
  EXPECT_EQ(0, sem_destroy(&my_semaphore));

  // Reinitialize a previously used semaphore, test 10 threads posting to 1
  // semaphore, 5 times each.
  EXPECT_EQ(0, sem_init(&my_semaphore, 0, 0));
  pta.iterations = 5;
  pthread_t my_thread_array[10];
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(0, pthread_create(&my_thread_array[i], 0, &PostThreadFunc, &pta));
  }
  // Wait 5*10 times, once per post:  5 posts for each of 10 posting-threads.
  for (int i = 0; i < 5*10; ++i) {
    EXPECT_EQ(0, sem_wait(&my_semaphore));
  }
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(0, pthread_join(my_thread_array[i], 0));
  }
  EXPECT_EQ(0, sem_destroy(&my_semaphore));

  // Reinitialize the previously used semaphore again, this time with a positive
  // starting value.
  EXPECT_EQ(0, sem_init(&my_semaphore, 0, 5));
  pta.iterations = 1;
  EXPECT_EQ(0, pthread_create(&my_thread, 0, &PostThreadFunc, &pta));
  // Wait 6 times, once for the post, 5 times for the initial starting value.
  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(0, sem_wait(&my_semaphore));
  }
  EXPECT_EQ(0, pthread_join(my_thread, 0));
  EXPECT_EQ(0, sem_destroy(&my_semaphore));
}

TEST_F(SemaphoreTests, TestSemTryWait) {

  int start_value = 10;
  sem_t sem;
  EXPECT_EQ(0, sem_init(&sem, 0, start_value));

  int value = -1;
  EXPECT_EQ(0, sem_getvalue(&sem, &value));
  EXPECT_EQ(10, value);
  // When the semaphore's value is positive, each call to
  // sem_trywait() should decrement the semaphore's value.
  for (int i = 1; i <= start_value; i++) {
    EXPECT_EQ(0, sem_trywait(&sem));
    EXPECT_EQ(0, sem_getvalue(&sem, &value));
    EXPECT_EQ(start_value - i, value);
  }
  // When the semaphore's value is zero, sem_trywait() should fail.
  EXPECT_EQ(-1, sem_trywait(&sem));
  EXPECT_EQ(EAGAIN, errno);
  EXPECT_EQ(0, sem_getvalue(&sem, &value));
  EXPECT_EQ(0, value);

  EXPECT_EQ(0, sem_destroy(&sem));

}

