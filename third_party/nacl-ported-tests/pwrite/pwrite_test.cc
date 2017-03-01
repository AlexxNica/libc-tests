/*
 * Copyright (c) 2013 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

static char const* g_dirname = "/tmp/pwrite_test";

/*
 * At least one test case bake in knowledge of relative sizes of these
 * quotes.
 */

static char const quote[] =
    "From a little after two oclock until almost sundown of the long still "
    "hot weary dead September afternoon they sat in what Miss Coldfield still "
    "called the office because her father had called it that---a dim hot "
    "airless room with the blinds all closed and fastened for forty-three "
    "summers because when she was a girl someone had believed that light and "
    "moving air carried heat and that dark was always cooler, and which (as "
    "the sun shone fuller and fuller on that side of the house) became "
    "latticed with yellow slashes full of dust motes which Quentin thought of "
    "as being flecks of the dead old dried paint itself blown inward from the "
    "scaling blinds as wind might have blown them.  There was a wistaria vine "
    "blooming for the second time that summer on a wooden trellis before one "
    "window, into which sparrows came now and then in random gusts, making a "
    "dry vivid dusty sound before going away: and opposite Quentin, Miss "
    "Coldfield in the eternal black which she had worn for forty-three years "
    "now, whether for her sister, father, or nothusband none knew, sitting so "
    "bolt upright in the straight hard chair that was so tall for her that "
    "her legs hung straight and rigid as if she had iron shinbones and "
    "ankles, clear of the floor with that air of impotent and static rage "
    "like children's feet, and talking in that grim haggard amazed voice "
    "until at last listening would renege and hearing-sense self-confound and "
    "the long-dead object of her impotent yet indomitable frustration wuld "
    "appear, as though by outraged recapitulation evoked, quiet inattentive "
    "and harmless, out of the biding and dreamy and victorious dust.";

static char const overwrite[] =
    "It was about eleven o'clock in the morning, mid October, with the sun "
    "not shining and a look of hard wet rain in the clearness of the "
    "foothills.  I was wearing my powder-blue suit, with dark blue shirt, tie "
    "and display handkerchief, black brogues, black wool socks with dark blue "
    "clocks on them.  I was neat, clean, shaved and sober, and I didn't care "
    "who knew it.  I was everything the well-dressed private detective ought "
    "to be.  I was calling on four million dollars.";

static char const first_seq_write[] =
    "Mr and Mrs Dursley, of number four, Privet Drive, were proud to say that "
    "they were perfectly normal, thank you very much. ";

static char const second_seq_write[] =
    "Not for the first time, an argument had broken out over breakfast at "
    "number four, Privet Drive.";

static void InitializeFile(std::string filename,
                           int* wr_fd,
                           int* rd_fd,
                           int wr_fd_open_flag) {
  FILE* iob = fopen(filename.c_str(), "w");
  size_t len = sizeof quote - 1;

  ASSERT_NE(nullptr, iob) << "Could not open " << filename
                          << " for initialization.";
  ASSERT_EQ(fwrite(quote, 1, len, iob), len)
      << "Could not initialize contents of " << filename;
  ASSERT_EQ(fclose(iob), 0)
      << "Could not close " << filename << " post initialization";
  *wr_fd = open(filename.c_str(), wr_fd_open_flag, 0);
  ASSERT_NE(*wr_fd, -1) << "Could not open " << filename
                        << " for test write access";
  *rd_fd = open(filename.c_str(), O_RDONLY, 0);
  ASSERT_NE(*rd_fd, -1) << "Could not open " << filename
                        << " for test read verfication access";
}

struct test_param {
  int wr_fd_open_flag;
  void* test_specifics;
};

class PwriteTests : public ::testing::TestWithParam<struct test_param> {
 protected:
  std::string test_file_name;
  int wr_fd = -1, rd_fd = -1;
  static int index;
  const struct test_param* params;

  void SetUp() override {
    params = &GetParam();
    test_file_name =
        g_dirname + std::string("/absalom") + std::to_string(++index) + ".txt";
    if (mkdir(g_dirname, 0600) != 0) {
      if (errno != EEXIST) {
        FAIL() << "Error while creating tmp dir: " << g_dirname
               << ", Error: " << errno;
      }
    }
    InitializeFile(test_file_name, &wr_fd, &rd_fd, params->wr_fd_open_flag);
    if (HasFatalFailure()) {
      FAIL();
    }
  }

  void TearDown() override {
    if (wr_fd != -1) {
      close(wr_fd);
    }
    if (rd_fd != -1) {
      close(rd_fd);
    }
    if (unlink(test_file_name.c_str()) != 0) {
      FAIL() << "Could not remove test file: " << test_file_name;
    }

    if (rmdir(g_dirname) != 0) {
      FAIL() << "Could not destroy test directory: " << g_dirname;
    }
  }
};

int PwriteTests::index = 0;

static void VerifyContents(int rd_fd,
                           char const* expected_data,
                           size_t nbytes,
                           off_t offset) {
  char buffer[4096];
  size_t chunk_size;
  ssize_t result;

  while (nbytes > 0) {
    chunk_size = nbytes;
    if (chunk_size > sizeof buffer) {
      chunk_size = sizeof buffer;
    }
    result = pread(rd_fd, buffer, chunk_size, offset);
    auto err = errno;
    ASSERT_EQ(result, (ssize_t)chunk_size) << "VerifyContents: pread failed"
                                           << ", errno " << err;
    ASSERT_EQ(memcmp(expected_data, buffer, chunk_size), 0)
        << "VerifyContents: unexpected data starting at offset " << offset <<
        " Got: " << buffer << " Expected: " << expected_data;
    expected_data += chunk_size;
    nbytes -= chunk_size;
    offset += chunk_size;
  }
}

class PwriteObeysOffsetTests : public PwriteTests {};

TEST_P(PwriteObeysOffsetTests, PwriteObeysOffset) {
  char buffer[4096];
  ssize_t nbytes;

  static_assert(sizeof buffer >= sizeof overwrite - 1,
                "configuration error: buffer too small");

  ASSERT_NE(lseek(wr_fd, 0, SEEK_END), (off_t)-1)
      << "seek to end before write failed";
  ASSERT_EQ(write(wr_fd, "Z", 1), 1) << "write at end failed";
  nbytes = pwrite(wr_fd, overwrite, sizeof overwrite - 1, (off_t)0);
  ASSERT_EQ(nbytes, (ssize_t)(sizeof overwrite - 1))
      << "pwrite to beginning failed";
  VerifyContents(rd_fd, overwrite, sizeof overwrite - 1, 0);
  VerifyContents(rd_fd, quote + sizeof overwrite - 1,
                 sizeof quote - sizeof overwrite, sizeof overwrite - 1);
  VerifyContents(rd_fd, "Z", 1, sizeof quote - 1);
}

struct test_param params1[] = {
    {O_RDWR, NULL},
    {O_WRONLY, NULL},
    {O_RDWR | O_APPEND, NULL},
    {O_WRONLY | O_APPEND, NULL},
};

INSTANTIATE_TEST_CASE_P(PwriteObeysOffset,
                        PwriteObeysOffsetTests,
                        ::testing::ValuesIn(params1));

class PwriteDoesNotAffectReadPosTests : public PwriteTests {};

TEST_P(PwriteDoesNotAffectReadPosTests, PwriteDoesNotAffectReadPos) {
  size_t const overwrite_overlap = 100;
  size_t read_count;
  char buffer[4096];
  ssize_t nbytes;

  read_count = ((sizeof overwrite - 1) - overwrite_overlap);
  nbytes = read(wr_fd, buffer, read_count);
  auto err = errno;
  ASSERT_EQ((size_t)nbytes, read_count) << "Pinitial read to set file pos"
                                           " failed "
                                        << " errno " << err;
  ASSERT_EQ(memcmp(buffer, quote, read_count), 0)
      << "Pinitial read result unexpected";
  nbytes = pwrite(wr_fd, overwrite, sizeof overwrite - 1, 0);
  ASSERT_EQ(nbytes, (ssize_t)(sizeof overwrite - 1)) << "Ppwrite failed";
  read_count = sizeof quote - 1 - read_count;
  nbytes = read(wr_fd, buffer, read_count);
  err = errno;
  ASSERT_EQ(nbytes, (ssize_t)read_count) << "Pread after pwrite failed,"
                                         << " errno " << err;
  ASSERT_EQ(memcmp(overwrite + sizeof overwrite - 1 - overwrite_overlap, buffer,
                   overwrite_overlap),
            0)
      << "Poverwritten data did not show up.";
  ASSERT_EQ(memcmp(buffer + overwrite_overlap, quote + sizeof overwrite - 1,
                   read_count - overwrite_overlap),
            0)
      << "Pdata beyond overwritten changed.";
  VerifyContents(rd_fd, overwrite, sizeof overwrite - 1, 0);
  VerifyContents(rd_fd, quote + sizeof overwrite - 1,
                 sizeof quote - sizeof overwrite, sizeof overwrite - 1);
}

struct test_param params2[] = {
    {O_RDWR, NULL},
    {O_RDWR | O_APPEND, NULL},
};

INSTANTIATE_TEST_CASE_P(PwriteDoesNotAffectReadPos,
                        PwriteDoesNotAffectReadPosTests,
                        ::testing::ValuesIn(params2));

class PwriteDoesNotAffectWritePosTests : public PwriteTests {};

TEST_P(PwriteDoesNotAffectWritePosTests, PwriteDoesNotAffectWritePos) {
  /*
   * Write first_seq_write to file, overwriting initial contents.
   * Next, pwrite to some a non-overlapping location (end of file).
   * Write second_seq_write to file, expecting that it would follow
   * the contents of first_seq_write.  Read file to verify.
   */
  ssize_t result;
  size_t seq_bytes;
  int expect_append = (int)(intptr_t)params->test_specifics;

  result = write(wr_fd, first_seq_write, sizeof first_seq_write - 1);
  auto err = errno;
  ASSERT_EQ(result, (ssize_t)(sizeof first_seq_write - 1))
      << "writing first_seq_write failed: "
      << " errno " << err;
  result = pwrite(wr_fd, overwrite, sizeof overwrite - 1, sizeof quote - 1);
  err = errno;
  ASSERT_EQ(result, (ssize_t)(sizeof overwrite - 1))
      << "pwriting overwrite failed:"
      << " errno " << err;
  result = write(wr_fd, second_seq_write, sizeof second_seq_write - 1);
  err = errno;
  ASSERT_EQ(result, (ssize_t)(sizeof second_seq_write - 1))
      << "writing second_seq_write failed:"
      << " errno " << err;
  if (expect_append) {
    VerifyContents(rd_fd, quote, sizeof quote - 1, 0);
    VerifyContents(rd_fd, overwrite, sizeof overwrite - 1, sizeof quote - 1);
    VerifyContents(rd_fd, second_seq_write, sizeof second_seq_write - 1,
                   sizeof quote - 1 + sizeof overwrite - 1);
  } else {
    VerifyContents(rd_fd, first_seq_write, sizeof first_seq_write - 1, 0);
    VerifyContents(rd_fd, second_seq_write, sizeof second_seq_write - 1,
                   sizeof first_seq_write - 1);
    seq_bytes = sizeof first_seq_write - 1 + sizeof second_seq_write - 1;
    VerifyContents(rd_fd, quote + seq_bytes, sizeof quote - 1 - seq_bytes,
                   seq_bytes);
    VerifyContents(rd_fd, overwrite, sizeof overwrite - 1, sizeof quote - 1);
  }
}

struct test_param params3[] = {
    {O_RDWR, NULL},
    {O_WRONLY, NULL},
    {O_RDWR | O_APPEND, (void*)1},
    {O_WRONLY | O_APPEND, (void*)1},
};

INSTANTIATE_TEST_CASE_P(PwriteDoesNotAffectWritePos,
                        PwriteDoesNotAffectWritePosTests,
                        ::testing::ValuesIn(params3));

class PreadObeysOffsetAndDoesNotAffectReadPtrTests : public PwriteTests {};

TEST_P(PreadObeysOffsetAndDoesNotAffectReadPtrTests,
       PreadObeysOffsetAndDoesNotAffectReadPtr) {
  ssize_t io_rv;
  static size_t const kFirstReadBytes = 128;
  static off_t const kPreadOffset = 256;
  static size_t const kPreadBytes = 128;
  char buffer[4096];

  static_assert(kFirstReadBytes <= sizeof buffer, "");
  static_assert(kFirstReadBytes <= sizeof quote - 1, "");
  io_rv = read(wr_fd, buffer, kFirstReadBytes);
  ASSERT_EQ(io_rv, (ssize_t)kFirstReadBytes) << "first read failed";
  ASSERT_EQ(memcmp(buffer, quote, kFirstReadBytes), 0)
      << "first read contents bad";
  static_assert(kPreadBytes <= sizeof buffer, "");
  io_rv = pread(wr_fd, buffer, kPreadBytes, kPreadOffset);
  auto err = errno;
  ASSERT_EQ(io_rv, (ssize_t)kPreadBytes) << "errno " << err;
  ASSERT_EQ(memcmp(buffer, quote + kPreadOffset, kPreadBytes), 0)
      << "pread content bad";
  static_assert(sizeof quote - 1 - kFirstReadBytes <= sizeof buffer, "");
  io_rv = read(wr_fd, buffer, sizeof buffer);
  ASSERT_EQ(io_rv, (ssize_t)(sizeof quote - 1 - kFirstReadBytes))
      << " bad byte count on 2nd read";
  ASSERT_EQ(memcmp(buffer, quote + kFirstReadBytes,
                   sizeof quote - 1 - kFirstReadBytes),
            0)
      << "2nd read contents bad";
}

struct test_param params4[] = {
    {O_RDONLY, NULL},
    {O_RDWR, NULL},
    {O_RDWR | O_APPEND, NULL},
};

INSTANTIATE_TEST_CASE_P(PreadObeysOffsetAndDoesNotAffectReadPtr,
                        PreadObeysOffsetAndDoesNotAffectReadPtrTests,
                        ::testing::ValuesIn(params4));
