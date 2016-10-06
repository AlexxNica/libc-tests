/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gtest/gtest.h"

// TODO: Tests are broken as append mode is not working, fix it
namespace {

class FileTests : public ::testing::Test {
 protected:
  const char* tmpDir = "/tmp/file_test";

  void SetUp() override {
    if (mkdir(tmpDir, 0600) != 0) {
      if (errno != EEXIST) {
        FAIL() << "Error while creating tmp dir: " << tmpDir
               << ", Error: " << errno;
      }
    }
    if (chdir(tmpDir) != 0) {
      FAIL() << "Not able to change dir to " << tmpDir;
    };
    // Remove test files from previous run
    remove("testdata256");
    remove("testdata.txt");
  }

  void TearDown() override {
    remove("testdata256");
    remove("testdata.txt");
    if (chdir("..") != 0) {
      FAIL() << "Not able to change dir to parent";
    };
    unlink(tmpDir);
  }
};

}  // namespace

#define TEXT_LINE_SIZE 1024


/*
 * global text for text file generation and
 * testing.  table is NULL terminated.
 */

static const char *gText[] = {
  "Text output test\n",
  "This is test text for text IO\n",
  "Duplicate line\n",
  "Duplicate line\n",
  "Same as above\n",
  "0123456789~!@#$%^&*()_+\n",
  NULL
};


/*
 * function createBinaryTestData(testname)
 *   Create binary 'testdata256' file.
 *   This file contains bytes 0..255
 *   It is exactly 256 bytes in size.
 */

void fopen_createBinaryFile(const char* testname) {
  // create and write 0..255 into binary testdata file

  FILE *f = fopen("testdata256", "wb");
  ASSERT_NE(nullptr, f) << "failed on fopen() for " << testname;
  for (int i = 0; i < 256; ++i) {
    unsigned char c = (unsigned char)i;
    size_t j = fwrite(&c, 1, 1, f);
    if (1 != j) {
      fclose(f);
      ASSERT_TRUE(false) << "can't fwrite(&c, 1, 1, f) for " << testname;
    }
  }
  fclose(f);
}


/*
 * function fread_bytes(testname, filename)
 *   Read and compare expected bytes from binary filename.
 *   Read is done byte at a time.
 *   The testdata256 file is exactly 256 bytes in length.
 *   We expect to find 0, 1, 2... 253, 254, 255.
 */

void fread_bytes(const char* testname, const char* filename) {
  FILE *f = fopen(filename, "rb");
  ASSERT_NE(nullptr, f) << "failed on fopen() for " << testname;
  for (int i = 0; i < 256; ++i) {
    unsigned char c;
    size_t j = fread(&c, 1, 1, f);
    if (1 != j) {
      fclose(f);
      ASSERT_TRUE(false) << "couldn't fread(&c, 1, 1, f) for " << testname;
    }
    if (c != (unsigned char)i) {
      fclose(f);
      ASSERT_TRUE(false) << "read bytes don't match for " << testname;
    }
  }
  fclose(f);
}


/*
 * function fopen_fail(testname)
 *   Attempt to fopen a file that isn't there.
 *   This should fail(!)
 */

void fopen_fail(const char* testname) {
  FILE *f = fopen("noexist.abc", "rb");

  if (NULL != f) {
    fclose(f);
    ASSERT_TRUE(false) << "fopen() succeeded on non-existant file! for "
                       << testname;
  }
  ASSERT_EQ(errno, ENOENT) << "fopen() wrong error on non-existant file! for "
                           << testname;
}


/*
 * function fread_256x1_block(testname)
 *   read and compare expected bytes from binary file
 *   read as one 256x1 chunk
 *   The testdata256 file is exactly 256 bytes in length.
 *   We expect to find 0, 1, 2... 253, 254, 255.
 */

void fread_256x1_block(const char* testname) {
  FILE *f = fopen("testdata256", "rb");
  ASSERT_NE(nullptr, f) << "failed on fopen() for " << testname;
  unsigned char c[256];
  memset(c, 0, sizeof(unsigned char) * 256);
  // read as 256x1 chunk
  size_t j = fread(&c, 256, 1, f);
  fclose(f);
  ASSERT_EQ(1ul, j) << "couldn't fread(&c, 256, 1, f) for " << testname;
  for (int i = 0; i < 256; ++i) {
    ASSERT_EQ(c[i], (unsigned char)i) << "read bytes don't match for "
                                      << testname;
  }
}


/*
 * function fread_1x256_block(testname)
 *   read and compare expected bytes from binary file
 *   read as one 1x256 chunks
 *   The testdata256 file is exactly 256 bytes in length.
 *   We expect to find 0, 1, 2... 253, 254, 255.
 */

void fread_1x256_block(const char* testname) {
  FILE *f = fopen("testdata256", "rb");
  ASSERT_NE(nullptr, f) << "failed on fopen() for " << testname;
  unsigned char c[256];
  memset(c, 0, sizeof(unsigned char) * 256);
  // read as 1x256 chunk
  size_t j = fread(&c, 1, 256, f);
  fclose(f);
  ASSERT_EQ(256ul, j) << "couldn't fread(&c, 1, 256, f) for " << testname;
  for (int i = 0; i < 256; ++i) {
    ASSERT_EQ(c[i], (unsigned char)i) << "read bytes don't match for "
                                      << testname;
  }
}


/*
 * function fopen_createTextFile(testname)
 *   create and write text file
 *   Uses 'gText' NULL terminated array to populate text file
 */

void fopen_createTextFile(const char* testname) {
  FILE *f = fopen("testdata.txt", "wt");
  ASSERT_NE(nullptr, f) << "fopen() testdata.txt (wt) for " << testname;

  for (int i = 0; NULL != gText[i]; ++i) {
    int numWritten = fprintf(f, "%s", gText[i]);
    if (numWritten < 0) {
      fclose(f);
      ASSERT_TRUE(false) << "fprintf() returned a negative value for "
                         << testname;
    }
  }
  fclose(f);
}


/*
 * function fgets_readText(testname)
 *   Read and compare expected text from test file.
 *   The test file should contain the same text as
 *   the 'gText' NULL terminated array.
 */

void fgets_readText(const char* testname) {
  FILE *f = fopen("testdata.txt", "rt");
  ASSERT_NE(nullptr, f) << "Unable to open file for " << testname;
  char buffer[TEXT_LINE_SIZE];
  int index;
  memset(buffer, 0, sizeof(char) * TEXT_LINE_SIZE);
  for (index = 0; NULL != fgets(buffer, TEXT_LINE_SIZE - 1, f); ++index) {
    if (NULL == gText[index]) {
      fclose(f);
      ASSERT_TRUE(false) << "unexpected mismatch for " << testname;
    }
    if (0 != strcmp(buffer, gText[index])) {
      fclose(f);
      ASSERT_TRUE(false) << "read text does not match for " << testname;
    }
  }
  fclose(f);
  ASSERT_EQ(nullptr, gText[index]) << "unexpected eof() encountered for "
                                   << testname;
}


/*
 * function fseek_filesize256(testname)
 *   Use fseek to determine expected filesize of test file filedata256.
 *   It is expected to be exactly 256 bytes in size.
 */

void fseek_filesize256(const char* testname) {
  FILE *f = fopen("testdata256", "rb");
  ASSERT_NE(nullptr, f) << "fopen failed for " << testname;
  int x = fseek(f, 0, SEEK_END);
  if (0 == x) {
    int p = ftell(f);
    if (256 != p) {
      fclose(f);
      ASSERT_TRUE(false) << "ftell mismatch for " << testname;
    }
    fclose(f);
  } else {
    fclose(f);
    ASSERT_TRUE(false) << "fseek failed for " << testname;
  }
}


/*
 * function fseek_simple_testdata256(testname)
 *   Verify a simple combination of fseek & fread.
 *   The testdata256 file is exactly 256 bytes in length,
 *   and consists of 0, 1, 2... 253, 254, 255.
 *   If we seek to the Nth byte, we expect to find
 *   the value N there.
 */

void fseek_simple_testdata256(const char* testname) {
  // test simple file seeking within testdata256

  FILE *f = fopen("testdata256", "rb");
  ASSERT_NE(nullptr, f) << "fseek_simple_testdata256() could not fopen() for "
                        << testname;

  // seek to offset 1 from start of file
  int x = fseek(f, 1, SEEK_SET);
  if (x != 0) {
    fclose(f);
    ASSERT_TRUE(false) << "fseek(1, SEEK_SET) failed for " << testname;
  }
  unsigned char c = 0;
  fread(&c, 1, 1, f);
  if (1 != c) {
    fclose(f);
    ASSERT_TRUE(false) << "fread(f, 1, 1, &c) at SEEK_SET+1 mismatch for "
                       << testname;
  }
  x = fseek(f, -2, SEEK_END);
  if (0 != x) {
    fclose(f);
    ASSERT_TRUE(false) << "fseek(-1, SEEK_END) failed for " << testname;
  }
  fread(&c, 1, 1, f);
  if (254 != c) {
    fclose(f);
    ASSERT_TRUE(false) << "fread(f, 1, 1, &c) at SEEK_END-1 mismatch for "
                       << testname;
  }
  x = fseek(f, 2, SEEK_SET);
  if (0 != x) {
    fclose(f);
    ASSERT_TRUE(false) << "fseek(2, SEEK_SET) failed for " << testname;
  }
  x = fseek(f, 1, SEEK_CUR);
  if (0 != x) {
    fclose(f);
    ASSERT_TRUE(false) << "fseek(1, SEEK_CUR) failed for " << testname;
  }
  fread(&c, 1, 1, f);
  if (3 != c) {
    fclose(f);
    ASSERT_TRUE(false) << "fseek(f, 1, 1, &c) at SEEK_CUR+1 mismatch for "
                       << testname;
  }
}


/*
 * function fgets_filesize(testname)
 *   Determine the length of a text file using fgets & strlen
 *   This value should match the length of gText[]
 */

void fgets_filesize(const char* testname) {
  size_t charcount = 0;
  size_t filecharcount = 0;

  // count chars in gText[]
  for (int i = 0; NULL != gText[i]; ++i) {
    charcount += strlen(gText[i]);
  }

  FILE *f = fopen("testdata.txt", "r");
  ASSERT_NE(nullptr, f) << "fgets_filesize() failed on fopen() for "
                        << testname;

  char buffer[TEXT_LINE_SIZE];
  memset(buffer, 0, sizeof(char) * TEXT_LINE_SIZE);
  while(NULL != fgets(buffer, TEXT_LINE_SIZE - 1, f)) {
    filecharcount += strlen(buffer);
  }
  fclose(f);

  ASSERT_EQ(charcount, filecharcount) << "fgets_filesize() mismatch for "
                                      << testname;
}


/*
 * function fopen_appendBinaryFile(testname)
 *   First create half a binary test file.
 *   Then re-open it for binary append, and add the other half.
 *   The intention is for testdata256 to contain 0, 1, 2... 253, 254, 255.
 */

void fopen_appendBinaryFile(const char* testname) {
  // first half: create a small file
  // write 0..99

  FILE *f = fopen("testdata256", "wb");
  ASSERT_NE(nullptr, f) << "failed on fopen() for " << testname;

  for (int i = 0; i < 100; ++i) {
    unsigned char c = (unsigned char)i;
    size_t j = fwrite(&c, 1, 1, f);
    if (1 != j) {
      fclose(f);
      ASSERT_TRUE(false) << "can't fwrite(&c, 1, 1, f) for " << testname;
    }
  }
  fclose(f);

  // second half: re-open and append the rest
  // write 99..255

  f = fopen("testdata256", "ab");
  ASSERT_NE(nullptr, f) << "fopen() testdata256 (ab) for " << testname;

  for (int i = 100; i < 256; ++i) {
    unsigned char c = (unsigned char)i;
    size_t j = fwrite(&c, 1, 1, f);
    if (1 != j) {
      fclose(f);
      ASSERT_TRUE(false) << "can't fwrite(&c, 1, 1, f) for " << testname;
    }
  }
  fclose(f);
}


/*
 * function fopen_appendTextFile(testname)
 *   First create a text file with first 2 lines.
 *   Then re-open it for append, and add the remaining lines.
 *   The intention is for testdata to contain gText[].
 */

void fopen_appendTextFile(const char* testname) {
  // create & write first 2 lines of gText[]

  FILE *f = fopen("testdata.txt", "w");
  int i = 0;
  ASSERT_NE(nullptr, f) << "fopen() testdata.txt (w+) for " << testname;

  EXPECT_TRUE((gText[0]) && (gText[1])) << "gText table is too small! for "
                                        << testname;
  if ((gText[0]) && (gText[1])) {
    // write first two lines
    fprintf(f, "%s", gText[i++]);
    fprintf(f, "%s", gText[i++]);
  }
  fclose(f);

  // re-open and append the rest of gText[]

  f = fopen("testdata.txt", "a");
  ASSERT_NE(nullptr, f) << "fopen() testdata.txt (w+) for " << testname;
  while (gText[i]) {
    fprintf(f, "%s", gText[i]);
    i++;
  }
  fclose(f);
}



/*
 * function test*()
 *
 *   Simple tests follow below.  Each test may call one or more
 *   of the functions above.  They all have a boolean return value
 *   to indicate success (all tests passed) or failure (one or more
 *   tests failed)  Order matters - the parent should call
 *   test1() before test2(), and so on.
 */

void test1() {
  // test the creation of a binary file
  fopen_createBinaryFile("test1");
}

void test2() {
  fopen_createTextFile("test2");
}

void test3() {
  // test reading bytes from binary file
  fread_bytes("test3", "testdata256");
}

void test4() {
  // test reading block from binary file
  fread_256x1_block("test4");
}

void test5() {
  // test reading block from binary file
  fread_1x256_block("test5");
}

void test6() {
  // test reading from text file
  fgets_readText("test6");
}

void test7() {
  // test binary file size
  fseek_filesize256("test7");
}

void test8() {
  // test text file size
  fgets_filesize("test8");
}

void test9() {
  // create binary file twice
  fopen_createBinaryFile("test7a");
  fopen_createBinaryFile("test7b");
}

void test10() {
  // create text file twice
  fopen_createTextFile("test8a");
  fopen_createTextFile("test8b");
}

void test11() {
  // verify binary file size again
  fseek_filesize256("test11");
}

void test12() {
  // verify test file size again
  fgets_filesize("test12");
}

void test13() {
  // verify seeks followed by reads
  fseek_simple_testdata256("test13");
}

void test14() {
  // create and then append to binary file
  // then verify contents & filesize
  fopen_appendBinaryFile("test14a");
  fread_bytes("test14b", "testdata256");
  fseek_filesize256("test14c");
}

void test15() {
  // create and then append to a text file
  // then verify contents & filesize
  fopen_appendTextFile("test15a");
  fgets_readText("test15b");
  fgets_filesize("test15c");
}

void test16() {
  // try a slightly different path
  fread_bytes("test16", "./testdata256");
}

void test17() {
  // try a slightly different path
  fread_bytes("test17", ".././file_test/testdata256");
}

void test18() {
  fopen_fail("test18");
}

TEST_F(FileTests, Test) {
  // The order of executing these tests matters!
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  test13();
  test14();
  test15();
  test16();
  test17();
  test18();
}
