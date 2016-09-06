/*
 * Copyright (c) 2013 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Simple test to verify that memcpy, memmove and memset are found and
 * work properly.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

enum {
/* A size between 8 and 256 bytes */
  medium_length = 8 * sizeof(long),

  large_length = 256,
/* A size big enough to hold 2 copies of the large length plus extra to test
 *  an unaligned destination.
 */
  total_buf_len = large_length * 2 + medium_length,
};

namespace {

class MemCopyMoveTests : public ::testing::Test {
 protected:

  /*
   * Create buffer as an array of long, to ensure word-size alignment. The
   * actual code accesses it via a char*.
   */
  volatile long buf[total_buf_len / sizeof(long)] = {0};
  unsigned char *arrptr;
  unsigned char *src, *dst;

  /*
   * Reset global buf to the sequence of bytes: 0, 1, 2 ... LENGTH - 1
   */
  void reset_buf(void) {
    unsigned char *bufptr = (unsigned char *) buf;
    unsigned i;
    for (i = 0; i < total_buf_len; ++i)
      bufptr[i] = i;
  }

  MemCopyMoveTests() {
    reset_buf();
    // You can do set-up work for each test here.
  }

  ~MemCopyMoveTests() override {
  }


  void SetUp() override {
    /* arrptr is an aligned pointer to the buffer. */
    arrptr = (unsigned char*) buf;
    if ((long) arrptr & (sizeof(long) - 1)) {
      FAIL() << "Internal error: unaligned buf";
    }
  }

  void TearDown() override {
  }

  void dump_buf() {
    unsigned char *bufptr = (unsigned char *) buf;
    for (int i = 0; i < total_buf_len; ++i)
      printf("buf[%u] (%p) = %u\n", i, (void *) &bufptr[i], bufptr[0]);
  }

  /*
   * Each function we're testing has a "checked" version that runs it and makes
   * sure the destination pointer is returned correctly. For memcpy, additionally
   * check that the source and the destination match after the call.
   */
  void checked_memcpy(void *dst, void *src, unsigned n) {
    ASSERT_LT((unsigned char *)dst + n, (unsigned char *)buf + total_buf_len);
    void *ret = memcpy(dst, src, n);
    char formatted[256];
    if (ret != dst) {
      dump_buf();
      sprintf(formatted, "Wrong memcpy return value: %p != %p\n", ret, dst);
      FAIL() << formatted;
    }
    if (memcmp(dst, src, n)) {
      dump_buf();
      sprintf(formatted, "memcmp after memcpy failure: %p -> %p len %u\n", src, dst, n);
      FAIL() << formatted;
    }
  }

  void checked_memmove(void *dst, void *src, unsigned n) {
    void *ret = memmove(dst, src, n);
    if (ret != dst) {
      char formatted[64];
      sprintf(formatted, "Wrong memmove return value: %p != %p\n", ret, dst);
      FAIL() << formatted;
    }
  }

  void checked_memset(void *s, int c, unsigned n) {
    void *ret = memset(s, c, n);
    char formatted[64];
    if (ret != s) {
      sprintf(formatted, "Wrong memset return value: %p != %p\n", ret, s);
      FAIL() << formatted;
    }
    char *s_char = (char *)s;
    char *dst_char = s_char;
    for (unsigned i = 0; i < n; ++i, ++dst_char) {
      if (*dst_char != c) {
        dump_buf();
        sprintf(formatted, "memset failure: index %d (%p) = %u\n",
               i, (void *) dst_char, *dst_char);
        FAIL() << formatted;
      }
    }
    if (*dst_char == c) {
      sprintf(formatted, "memset failure: wrote %d past the end of buffer\n", c);
      FAIL() << formatted;
    }
  }
};

} //namespace

/*
 * Test 1: memcpy small chunk, from aligned to aligned address.
 * "small chunk" is anything smaller than UNROLLBLOCKSIZE in our
 * implementation of these functions.
 */
TEST_F(MemCopyMoveTests, TestMemcpySmallChunkAtoA) {
  src = arrptr;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, 6);
  EXPECT_EQ(4u, (unsigned)dst[4]);
}

/* Test 2: memcpy small chunk, from aligned to unaligned address */
TEST_F(MemCopyMoveTests, TestMemcpySmallChunkAtoU) {
  src = arrptr;
  dst = arrptr + medium_length * 2 + 1;
  checked_memcpy(dst, src, 6);
  EXPECT_EQ(4u, (unsigned)dst[4]);
}

/* Test 3: memcpy small chunk, from unaligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemcpySmallChunkUtoA) {
  src = arrptr + 1;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, 6);
  EXPECT_EQ(5u, (unsigned)dst[4]);
}

/* Test 4: memcpy small chunk, from unaligned to unaligned address */
TEST_F(MemCopyMoveTests, TestMemcpySmallChunkUtoU) {
  src = arrptr + 3;
  dst = arrptr + medium_length * 2 + 3;
  checked_memcpy(dst, src, 6);
  EXPECT_EQ(7u, (unsigned)dst[4]);
}

/* Test 5: memcpy medium chunk, from aligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemcpyMedChunkAtoA) {
  src = arrptr;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, medium_length);
  EXPECT_EQ(30u, (unsigned)dst[30]);
}

/* Test 6: memcpy medium chunk, from aligned to unaligned address */
TEST_F(MemCopyMoveTests, TestMemcpyMedChunkAtoU) {
  src = arrptr;
  dst = arrptr + medium_length * 2 + 1;
  checked_memcpy(dst, src, medium_length);
  EXPECT_EQ(30u, (unsigned)dst[30]);
}

/* Test 7: memcpy medium chunk, from unaligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemcpyMedChunkUtoA) {
  src = arrptr + 1;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, medium_length);
  EXPECT_EQ(31u, (unsigned)dst[30]);
}

/* Test 8: memcpy medium chunk, from unaligned to unaligned address */
TEST_F(MemCopyMoveTests, TestMemcpyMedChunkUtoU) {
  src = arrptr + 3;
  dst = arrptr + medium_length * 2 + 3;
  checked_memcpy(dst, src, medium_length);
  EXPECT_EQ(33u, (unsigned)dst[30]);
}

/* Test 9: memcpy medium chunk, near edges/overlap */
TEST_F(MemCopyMoveTests, TestMemcpyMedChunkNearEdge) {
  src = arrptr;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, medium_length * 2);
  EXPECT_EQ(10u, (unsigned)dst[10]);
}

/* Test 10: memcpy large chunk, from aligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemcpyLargeChunkAToA) {
  src = arrptr;
  dst = arrptr + large_length;
  checked_memcpy(dst, src, large_length);
  EXPECT_EQ(129u, (unsigned)dst[129]);
}

/* Test 11: memcpy large chunk, from unaligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemcpyLargeChunkUToA) {
  src = arrptr + 1;
  dst = arrptr + large_length;
  checked_memcpy(dst, src, large_length);
  EXPECT_EQ(130u, (unsigned)dst[129]);
}

/* Test 12: memcpy large chunk, from aligned to unaligned address */
TEST_F(MemCopyMoveTests, TestMemcpyLargeChunkAToU) {
  src = arrptr;
  dst = arrptr + large_length + 3;
  checked_memcpy(dst, src, large_length);
  EXPECT_EQ(129u, (unsigned)dst[129]);
}

/* Test 13: memcpy 0-sized chunk */
TEST_F(MemCopyMoveTests, TestMemcpyZeroSizedChunk) {
  src = arrptr;
  dst = arrptr + medium_length * 2;
  checked_memcpy(dst, src, 0);
  EXPECT_EQ(128u, (unsigned)dst[0]);
}

  /* Test 100: memset small chunk, aligned address */
TEST_F(MemCopyMoveTests, TestMemsetSmallChunkAlinged) {
  reset_buf();
  checked_memset(arrptr, 99, 5);
  EXPECT_EQ(99u, (unsigned)arrptr[4]);
}

/* Test 101: memset small chunk, unaligned address */
TEST_F(MemCopyMoveTests, TestMemsetSmallChunkiUnAlinged) {
  checked_memset(arrptr + 3, 99, 5);
  EXPECT_EQ(99u, (unsigned)arrptr[7]);
  EXPECT_EQ(2u, (unsigned)arrptr[2]);
  EXPECT_EQ(8u, (unsigned)arrptr[8]);
}

/* Test 102: memset medium chunk, aligned address */
TEST_F(MemCopyMoveTests, TestMemsetMedChunkAlinged) {
  checked_memset(arrptr, 99, medium_length);
  EXPECT_EQ(99u, (unsigned)arrptr[31]);
}

/* Test 103: memset medium chunk, unaligned address */
TEST_F(MemCopyMoveTests, TestMemsetMedChunkiUnAlinged) {
  checked_memset(arrptr + 3, 99, medium_length);
  EXPECT_EQ(99u, (unsigned)arrptr[34]);
}

/* Test 104: edge */
TEST_F(MemCopyMoveTests, TestMemsetEdgeCase) {
  checked_memset(arrptr, 99, medium_length * 2);
  EXPECT_EQ(99u, (unsigned)arrptr[medium_length * 2 - 1]);
}

/* Test 105: memset large chunk, aligned address */
TEST_F(MemCopyMoveTests, TestMemsetLargeChunckAlinged) {
  checked_memset(arrptr, 99, large_length);
  EXPECT_EQ(99u, (unsigned)arrptr[large_length - 1]);
}

/* Test 106: memset large chunk, unaligned address */
TEST_F(MemCopyMoveTests, TestMemsetLargeChunckUnAlinged) {
  checked_memset(arrptr + 3, 99, large_length);
  EXPECT_EQ(99u, (unsigned)arrptr[large_length + 2]);
}

/* Test 107: memset zero size */
TEST_F(MemCopyMoveTests, TestMemsetZeroSize) {
  checked_memset(arrptr, 99, 0);
  EXPECT_EQ(0u, (unsigned)arrptr[0]);
}

/*
 * The non-overlapping logic of memmove is pretty much the same as memcpy.
 * Do a sanity check and then test overlapping addresses.
 */

/* Test 201: memmove medium chunk, from aligned to aligned address */
TEST_F(MemCopyMoveTests, TestMemmoveMedChunckAotA) {
  src = arrptr;
  dst = arrptr + medium_length * 2;
  checked_memmove(dst, src, medium_length);
  EXPECT_EQ(31u, (unsigned)dst[31]);
}

/* Test 202: memmove small chunk in overlapping addresses */
TEST_F(MemCopyMoveTests, TestMemmoveSmallChunkOverlap) {
  src = arrptr + 4;
  dst = arrptr;
  checked_memmove(dst, src, 8);
  EXPECT_EQ(11u, (unsigned)dst[7]);
}

/* Test 203: memmove large chunk in overlapping addresses */
TEST_F(MemCopyMoveTests, TestMemmoveLargeChunkOverlap) {
  src = arrptr + 1;
  dst = arrptr;
  checked_memmove(dst, src, medium_length * 2);
  EXPECT_EQ(64u, (unsigned)dst[63]);
}

/* Test 204: memmove at edge */
TEST_F(MemCopyMoveTests, TestMemmoveEdgeCase) {
  src = arrptr + 1;
  dst = arrptr;
  checked_memmove(dst, src, medium_length * 4 - 1);
  /* expect length-1 */
  EXPECT_EQ((unsigned)(large_length -1), (unsigned)dst[medium_length * 4 - 2]);
}
