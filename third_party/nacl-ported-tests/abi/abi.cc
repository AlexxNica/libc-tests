/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Check ABI compliance, this is especially important for PNaCl.
 * This test should cover at least all the types that appear in
 * the core IRT interface (src/untrusted/irt/irt.h) as well as
 * the base types.
 */

#include <dirent.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "gtest/gtest.h"

namespace {

class AbiTests : public ::testing::Test {
 protected:

  AbiTests() {
    // You can do set-up work for each test here.
  }

  ~AbiTests() override {
  }


  void SetUp() override {
  }

  void TearDown() override {
  }
};

} //namespace


/* we expect this to be padded to 16 bytes of total size */
typedef struct {
  double d;
  float f;
} S1;

#define CHECK_SIZEOF(t, v) EXPECT_EQ(sizeof(t), (size_t)v) << "bad sizeof " #t

/* Helper Types so we can describe a type in one token */
typedef void (*FunctionPointer)(void);
typedef void *Pointer;
typedef long long long_long;
typedef long double long_double;

TEST_F(AbiTests, CheckSizes) {
  CHECK_SIZEOF(char, 1);
  CHECK_SIZEOF(short, 2);
  CHECK_SIZEOF(int, 4);
  CHECK_SIZEOF(long, 8);
  CHECK_SIZEOF(long long, 8);

  CHECK_SIZEOF(Pointer, 8);
  CHECK_SIZEOF(FunctionPointer, 8);

  CHECK_SIZEOF(float, 4);
  CHECK_SIZEOF(double, 8);
  CHECK_SIZEOF(long_double, 16);

  CHECK_SIZEOF(S1, 16);
  CHECK_SIZEOF(S1[2], 16*2);

  CHECK_SIZEOF(dev_t, 8);

  CHECK_SIZEOF(ino_t, 8);
  CHECK_SIZEOF(mode_t, 4);
  CHECK_SIZEOF(nlink_t, 8);
  CHECK_SIZEOF(uid_t, 4);
  CHECK_SIZEOF(gid_t, 4);
  CHECK_SIZEOF(off_t, 8);
  CHECK_SIZEOF(blksize_t, 8);
  CHECK_SIZEOF(blkcnt_t, 8);

  CHECK_SIZEOF(off_t, 8);
  CHECK_SIZEOF(size_t, 8);
  CHECK_SIZEOF(fpos_t, 16);

  CHECK_SIZEOF(time_t, 8);
  CHECK_SIZEOF(struct timezone, 8);
  CHECK_SIZEOF(suseconds_t, 8);
  CHECK_SIZEOF(clock_t, 8);
  CHECK_SIZEOF(clockid_t, 4);
  CHECK_SIZEOF(struct timeval, 16);
  CHECK_SIZEOF(struct timespec, 16);

  CHECK_SIZEOF(struct stat, 144);
  CHECK_SIZEOF(struct dirent, 280);

  CHECK_SIZEOF(jmp_buf, 208);
}


#define CHECK_ALIGNMENT(T, a) do { \
  typedef struct { char c; T  x; } AlignStruct; \
  EXPECT_EQ(offsetof(AlignStruct, x), (size_t)a) << "bad offsetof " #T; \
  } while(0)

TEST_F(AbiTests, CheckAlignment) {
  CHECK_ALIGNMENT(char, 1);
  CHECK_ALIGNMENT(short, 2);
  CHECK_ALIGNMENT(int, 4);
  CHECK_ALIGNMENT(long, 8);
  CHECK_ALIGNMENT(float, 4);
  CHECK_ALIGNMENT(double, 8);
  CHECK_ALIGNMENT(long_double, 16);
  CHECK_ALIGNMENT(Pointer, 8);
  CHECK_ALIGNMENT(FunctionPointer, 8);
  CHECK_ALIGNMENT(long_long, 8);

  CHECK_ALIGNMENT(struct stat, 8);
  CHECK_ALIGNMENT(struct dirent, 8);
  CHECK_ALIGNMENT(struct timeval, 8);
  CHECK_ALIGNMENT(struct timespec, 8);
}


int AssertMemcmp(const char* name, unsigned char* p1, unsigned char* p2, int n) {
  int i;
  if (0 ==  memcmp(p1, p2, n)) {
    return 0;
  }

  printf("ERROR: bad bitfield contents for %s\n", name);
  printf("%-10s: ", "actual");
  for (i = 0; i < n; ++i) {
    printf(" %02x", p1[i]);
  }
  printf("\n");

  printf("%-10s: ", "expected");
  for (i = 0; i < n; ++i) {
    printf(" %02x", p2[i]);
  }
  printf("\n");

  return 1;
}

#define CHECK_BITFIELD(s1, s2, s3, v1, v2, v3, n, s) do { \
    typedef struct { unsigned f1: s1; unsigned f2:  s2; unsigned f3: s3; } BF; \
    const char* name = #s1 ":" #s2 ":" #s3 "::" #v1 ":" #v2 ":" #v3; \
    BF bf;                          \
    memset(&bf, 0, sizeof(BF)); \
    bf.f1 = v1; bf.f2 = v2; bf.f3 = v3; \
    EXPECT_EQ(sizeof(bf), (size_t)n) << "unexpected bitfield size"; \
    EXPECT_EQ(AssertMemcmp(name, (unsigned char *)&bf, (unsigned char *)s, n), 0); \
  } while(0)


TEST_F(AbiTests, CheckBitfields) {
  CHECK_BITFIELD(1, 1, 1,  0, 0, 1,  4,
                 "\x04\0\0\0");
  CHECK_BITFIELD(2, 2, 2,  1, 2, 3,  4,
                 "\x39\0\0\0");
  CHECK_BITFIELD(3, 3, 3,  1, 2, 7,  4,
                 "\xd1\x01\0\0");
  CHECK_BITFIELD(7, 7, 7,  1, 2, 3,  4,
                 "\x01\xc1\0\0");
  CHECK_BITFIELD(11, 11, 11,  1, 2, 3,  8,
                 "\x01\x10\x00\x00\x03\x00\x00\x00");
  CHECK_BITFIELD(31, 31, 31,  1, 2, 3,  12,
                 "\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00");

}
