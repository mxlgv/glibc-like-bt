/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#include <stdarg.h>
#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#else
#include <execinfo.h>
#endif

#include "GlibcBt.h"
#include "Helper.h"

void TestMain(void) {
  void* addrs[DEPTH];
  void* addrs_expected[DEPTH];
  int depth, depth_expected;

#ifdef WIN32
  depth_expected = CaptureStackBackTrace(0, DEPTH, addrs_expected, NULL);
#else
  depth_expected = backtrace(addrs_expected, DEPTH);
#endif

  depth = GlibcBt_Backtrace(addrs, DEPTH);
  ASSERT(depth == depth_expected);

  for (int i = 1; i < depth_expected; i++) {
    ASSERT(addrs[i] == addrs_expected[i]);
  }
}

void DummyFunc6(void) { TestMain(); }

void DummyFunc5(void) { DummyFunc6(); }

void DummyFunc4(void) { DummyFunc5(); }

void DummyFunc3(void) { DummyFunc4(); }

void DummyFunc2(void) { DummyFunc3(); }

void DummyFunc1(void) { DummyFunc2(); }

int main(void) {
  DummyFunc1();
  return 0;
}
