// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>

const char *testdata_dir;
const char *base_dir;

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  base_dir = dirname(argv[0]);
  testdata_dir = strcat((char *)base_dir, "/testdata");
  return RUN_ALL_TESTS();
}
