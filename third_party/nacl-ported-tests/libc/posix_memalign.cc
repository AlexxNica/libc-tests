/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "gtest/gtest.h"

namespace {

class PosixMemAlignTests : public ::testing::Test {
 protected:

  PosixMemAlignTests() {
    // You can do set-up work for each test here.
  }

  ~PosixMemAlignTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace

TEST_F(PosixMemAlignTests, TestPosixMemAlign) {
  char *outp;
  /* Try a few alignments, some of them might be the standard malloc align.
   * They should all be powers of 2 and be a multiple of sizeof(void*).
   */
  size_t align_to_test[] = {8, 16, 32, 64, 128, 256, 512, 1024, 0};
  size_t sizes_to_test[] = {1, 2, 4, 8, 10, 16, 32, 64, 128, 256, 512, 1024, 0};
  int i = 0;
  int j = 0;

  while (align_to_test[i] != 0) {
    j = 0;
    while (sizes_to_test[j] != 0) {
      int err = posix_memalign((void **)&outp,
                               align_to_test[i],
                               sizes_to_test[j] * sizeof(char));
      ASSERT_EQ(err, 0) << "Input params were align=" << align_to_test[i]
                        << " size=" << sizes_to_test[j];
      ASSERT_EQ((size_t)outp % align_to_test[i], 0ul)
          << "posix_memalign failed to align to " << align_to_test[i]
          << ": ptr=" << (void *) outp;
      free(outp);
      j++;
    }
    i++;
  }

  /* Check that a non-power of 2 alignment fails. */
  ASSERT_EQ(posix_memalign((void**)&outp, 7, 20 * sizeof(char)), EINVAL)
      << "posix_memaligned failed to return EINVAL for non-pow2!";

  /* Check that smaller than sizeof(void*) alignment fails. */
  ASSERT_EQ(posix_memalign((void**)&outp, sizeof(void*) - 1,
                     20 * sizeof(char)), EINVAL)
      << "posix_memaligned failed to return EINVAL for non-pow2!";
}
