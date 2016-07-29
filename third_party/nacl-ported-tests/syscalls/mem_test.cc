/*
 * Copyright (c) 2010 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <sys/mman.h>

#include <cstdlib>
#include <cstring>
#include "gtest/gtest.h"

namespace {

class MemTests : public ::testing::Test {
 protected:

  // Use 64Kb regions in the memory tests.  This is usually a whole number of
  // pages.  The 32Kb size is used for certain other failure modes.
  const size_t k64Kbytes = 64 * 1024;
  const size_t k32Kbytes = 32 * 1024;

  // |MAP_ANONYMOUS| expects a filedesc of -1.  While this is not strictly true
  // on all systems, portable code should work this way.
  const int kAnonymousFiledesc = -1;

  // When |MAP_ANONYMOUS| is not specified, any negative file desc should report
  // EBADF.
  const int kBadFiledesc = -3;


  MemTests() {
    // You can do set-up work for each test here.
  }

  ~MemTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


// Try to mmap a 0-length region.  This is expected to fail.
TEST_F(MemTests, TestZeroLengthRegion) {
  void* mmap_ptr = mmap(NULL,
                        0,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        kAnonymousFiledesc,
                        0);
  EXPECT_EQ(MAP_FAILED, mmap_ptr);
  EXPECT_EQ(EINVAL, errno);
}

// Hand in a bad file descriptor, this test is successful if mmap() fails.
// The |MAP_ANONYMOUS| flag is deliberately not used for this test, because
// we are trying to map an actual (but bad) filedesc.
TEST_F(MemTests, TestBadFiledesc) {
  void* mmap_ptr = mmap(NULL,
                        k64Kbytes,
                        PROT_READ,
                        MAP_PRIVATE,
                        kBadFiledesc,
                        0);
  EXPECT_EQ(MAP_FAILED, mmap_ptr);
  EXPECT_EQ(EBADF, errno);
}

// Verify that mmap does not fail if a bad hint address is passed, but
// |MMAP_FIXED| is not specified.
TEST_F(MemTests, TestMmapBadHint) {
  void* bad_hint = (void *) 0x123;
  void* mmap_ptr = mmap(bad_hint,
                        k64Kbytes,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        kAnonymousFiledesc,
                        0);
  EXPECT_NE(MAP_FAILED, mmap_ptr);
  EXPECT_NE(mmap_ptr, bad_hint);
  EXPECT_EQ(munmap(mmap_ptr, k64Kbytes), 0);
}

// Verify that mmap does fail if a bad hint address is passed and
// |MMAP_FIXED| is specified.
TEST_F(MemTests, TestMmapBadHintFixed) {
  void* bad_hint = (void *) 0x123;
  void* mmap_ptr = mmap(bad_hint,
                        k64Kbytes,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                        kAnonymousFiledesc,
                        0);
  EXPECT_EQ(MAP_FAILED, mmap_ptr);
}

// Test mmap() and munmap(), since these often to go together.  Tries to mmap
// a 64 Kb region of memory and then tests to make sure that the pages have all
// been 0-filled.
TEST_F(MemTests, TestMmapMunmap) {
  void* mmap_ptr = mmap(NULL,
                        k64Kbytes,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        kAnonymousFiledesc,
                        0);
  EXPECT_NE(MAP_FAILED, mmap_ptr);
  // Create a zero-filled region on the heap for comparison with the mmaped
  // region.
  void* zeroes = std::malloc(k64Kbytes);
  std::memset(zeroes, 0, k64Kbytes);
  EXPECT_EQ(std::memcmp(mmap_ptr, zeroes, k64Kbytes), 0);
  // Attempt to release the mapped memory.
  EXPECT_EQ(munmap(mmap_ptr, k64Kbytes), 0);
  std::free(zeroes);
}

// Verify that mmap into the NULL pointer guard page will fail.  This uses the
// |MAP_FIXED| flag to try an force mmap() to pin the region at NULL.
TEST_F(MemTests, TestMmapNULL) {
  void *mmap_ptr = mmap(NULL,
                        k64Kbytes,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                        kAnonymousFiledesc,
                        0);
  EXPECT_EQ(MAP_FAILED, mmap_ptr);
  EXPECT_EQ(EINVAL, errno);
}

// Attempt to allocate 32Kb, starting at 32Kb.  This should fail.
TEST_F(MemTests, TestMmap32k) {
  void *mmap_ptr = mmap(reinterpret_cast<void*>(k32Kbytes),
                        k32Kbytes,
                        PROT_READ,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                        kAnonymousFiledesc,
                        0);
  EXPECT_EQ(MAP_FAILED, mmap_ptr);
  EXPECT_EQ(EINVAL, errno);
}

// Verify that mmap() with the |MAP_FIXED| flag and a non-page-aligned address
// will fail.
TEST_F(MemTests, TestMmapNonPageAligned) {
  // Reserve some legal (page-aligned) address space in which to perform the
  // test.
  char* local_heap = reinterpret_cast<char*>(mmap(NULL,
                                                  k64Kbytes,
                                                  PROT_NONE,
                                                  MAP_PRIVATE | MAP_ANONYMOUS,
                                                  kAnonymousFiledesc,
                                                  0));
  EXPECT_NE(MAP_FAILED, local_heap);

  void* unaligned_ptr = mmap(static_cast<void*>(local_heap + 0x100),
                             k64Kbytes,
                             PROT_READ,
                             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                             kAnonymousFiledesc,
                             0);
  EXPECT_EQ(MAP_FAILED, unaligned_ptr);
  EXPECT_EQ(EINVAL, errno);
}

