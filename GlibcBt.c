/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#include "GlibcBt.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32)
#include <processthreadsapi.h>
#endif

struct StackFrame {
  struct StackFrame* nextFrame;
  void* returnAddr;
};

static void Panic(const char* fmt, ...) {
  va_list vlist;
  va_start(vlist, fmt);
  vfprintf(stderr, fmt, vlist);
  va_end(vlist);
  exit(1);
}

static uintptr_t GetStackBottom(void) {
  void* stack_bottom_addr = NULL;
  void* dummy;

#ifdef WIN32
  GetCurrentThreadStackLimits((PULONG_PTR)&dummy,
                              (PULONG_PTR)&stack_bottom_addr);
#else
  char line[100];
  FILE* maps_file = fopen("/proc/self/maps", "r");
  if (!maps_file) {
    Panic("Can't open '/proc/self/maps'!");
  }

  while (fgets(line, sizeof(line), maps_file)) {
    if (strstr(line, "[stack]")) {
      sscanf(line, "%p-%p", &dummy, &stack_bottom_addr);
      break;
    }
  }

  fclose(maps_file);
#endif

  if (!stack_bottom_addr) {
    Panic("Stack bottom not found!");
  }

  return (uintptr_t)stack_bottom_addr;
}

static struct StackFrame* GetNextFrame(const struct StackFrame* stack_frame,
                                       uintptr_t stack_bottom_addr) {
  if ((uintptr_t)stack_frame->nextFrame < (uintptr_t)stack_frame ||
      (uintptr_t)stack_frame->nextFrame > stack_bottom_addr) {
    return NULL;
  }

  return stack_frame->nextFrame;
}

int GlibcBt_Backtrace(void** addrs, int depth) {
  int frame_count = 0;
  struct StackFrame* stack_frame = GlibcBt_GetFrameAddr();
  uintptr_t stack_bottom_addr = GetStackBottom();

  for (frame_count = 0;
       stack_frame && stack_frame->returnAddr && frame_count < depth;
       frame_count++) {
    addrs[frame_count] = stack_frame->returnAddr;
    stack_frame = GetNextFrame(stack_frame, stack_bottom_addr);
  }

  return frame_count;
}

char** GlibcBt_BacktraceSymbols(void** addrs, int depth) {
  (void)addrs;
  (void)depth;

  Panic("Function '%s' not implemented!\n", __func__);
  return NULL;
}

void GlibcBt_BacktraceSymbolsFd(void** addrs, int depth, int fd) {
  (void)addrs;
  (void)depth;
  (void)fd;

  Panic("Function '%s' not implemented!\n", __func__);
}
