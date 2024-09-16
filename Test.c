/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#define _GNU_SOURCE

#include <assert.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "GlibcBt.h"

// So that the assertion always works
#undef NDEBUG

// Request backtrace depth.
// Placed as a global variable so as not to pass it through multiple calls.
int depth_requested = 0;

void TestMain(void) {
  void **const addrs_expected =
      malloc(depth_requested * sizeof(*addrs_expected));
  assert(addrs_expected);
  void **const addrs = malloc(depth_requested * sizeof(*addrs));
  assert(addrs);

  // Test GlibcBt_Backtrace()
  const int depth_expected = backtrace(addrs_expected, depth_requested);
  const int depth = GlibcBt_Backtrace(addrs, depth_requested);

  printf("depth: %d == %d\n", depth, depth_expected);
  assert(depth == depth_expected);

  // The return addresses for backtrace() and GlibcBt_Backtrace() are different.
  // This is the only difference that is being fixed.
  addrs[0] = addrs_expected[0];

  for (int i = 0; i < depth_expected; i++) {
    printf("addrs: %p == %p\n", addrs[i], addrs_expected[i]);
    assert(addrs[i] == addrs_expected[i]);
  }

  // Test GlibcBt_BacktraceSymbols()
  char **const strs_expected =
      backtrace_symbols(addrs_expected, depth_expected);
  assert(strs_expected);
  char **const strs = GlibcBt_BacktraceSymbols(addrs, depth);
  assert(strs);

  for (int i = 0; i < depth_expected; i++) {
    printf("strs: %s == %s\n", strs[i], strs_expected[i]);
    assert(!strcmp(strs[i], strs_expected[i]));
  }

  free(strs);
  free(strs_expected);

  // Test GlibcBt_BacktraceSymbolsFd()
  const int fd_expected = memfd_create("test-backtrace-symbols-fd-expected", 0);
  assert(fd_expected != -1);
  const int fd = memfd_create("test-backtrace-symbols-fd", 0);
  assert(fd != -1);

  backtrace_symbols_fd(addrs_expected, depth_expected, fd_expected);
  GlibcBt_BacktraceSymbolsFd(addrs, depth, fd);

  const off_t size_expected = lseek(fd_expected, 0, SEEK_END);
  const off_t size = lseek(fd, 0, SEEK_END);
  assert(size == size_expected);

  lseek(fd_expected, 0, SEEK_SET);
  lseek(fd, 0, SEEK_SET);

  char *const buff = malloc(size);
  assert(buff);
  char *const buff_expected = malloc(size_expected);
  assert(buff_expected);

  assert(read(fd_expected, buff_expected, size_expected) == size_expected);
  assert(read(fd, buff, size) == size);
  close(fd);
  close(fd_expected);

  assert(!memcmp(buff, buff_expected, size_expected));

  free(buff_expected);
  free(buff);

  free(addrs_expected);
  free(addrs);
}

// Test call tree:
void DummyFunc6(void) { TestMain(); }

int DummyFunc5(void) {
  DummyFunc6();
  return 1;
}

void DummyFunc4(void) {
  int f = DummyFunc5();
  (void)f;
}

void DummyFunc3(int a, int b) {
  (void)a;
  (void)b;
  DummyFunc4();
}

void DummyFunc2(void) { DummyFunc3(4, 1); }

void DummyFunc1(void) { DummyFunc2(); }

int main(void) {
  setbuf(stdout, NULL);

  puts("Test normal depth:");
  depth_requested = 5;
  DummyFunc1();
  puts("OK");

  puts("Test overflowing depth:");
  depth_requested = 10000;
  DummyFunc1();
  puts("OK");

  return 0;
}
