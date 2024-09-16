/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#define _GNU_SOURCE

#include "GlibcBt.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>
#include <unistd.h>

const char *kFmtSymbols = "%s(%s+0x%" PRIxPTR ") [0x%" PRIxPTR "]";
const size_t kFmtSymbolsSize = sizeof("(+0x) [0x]");

const char *kFmtNoSymbols = "[0x%" PRIxPTR "]";
const size_t kFmtNoSymbolsSize = sizeof("[0x]");

struct StackFrame {
  struct StackFrame *next_frame;
  void *ret_addr;
};

struct Symbol {
  Dl_info info;
  uintptr_t ret_addr_offset;
  size_t pre_calc_str_size;
  int found;
};

static uintptr_t GetStackBottom(void) {
  uintptr_t stack_bottom_addr = 0;
  uintptr_t dummy;

  char line[100];
  FILE *const maps_file = fopen("/proc/self/maps", "r");
  if (!maps_file) {
    return stack_bottom_addr;
  }

  while (fgets(line, sizeof(line), maps_file)) {
    if (strstr(line, "[stack]")) {
      sscanf(line, "%" SCNxPTR "-%" SCNxPTR, &dummy, &stack_bottom_addr);
      break;
    }
  }

  fclose(maps_file);

  return stack_bottom_addr;
}

static struct StackFrame *GetNextFrame(const struct StackFrame *stack_frame,
                                       uintptr_t stack_bottom_addr) {
  if ((uintptr_t)stack_frame->next_frame < (uintptr_t)stack_frame ||
      (uintptr_t)stack_frame->next_frame > stack_bottom_addr) {
    return NULL;
  }

  return stack_frame->next_frame;
}

int GlibcBt_Backtrace(void **addrs, int depth) {
  int frame_count;
  struct StackFrame *stack_frame = GlibcBt_GetFrameAddr();
  const uintptr_t stack_bottom_addr = GetStackBottom();
  if (!stack_bottom_addr) {
    return 0;
  }

  for (frame_count = 0; stack_frame && frame_count < depth; frame_count++) {
    addrs[frame_count] = stack_frame->ret_addr;
    stack_frame = GetNextFrame(stack_frame, stack_bottom_addr);
  }

  return frame_count;
}

static size_t CalcHexDigits(uintptr_t ptr) {
  size_t i;
  for (i = 0; ptr; i++) {
    ptr <<= 4;
  }

  return i;
}

char **GlibcBt_BacktraceSymbols(void *const *addrs, int depth) {
  struct Symbol *const syms = malloc(depth * sizeof(struct Symbol));
  if (!syms) {
    return NULL;
  }

  size_t strs_area_size = 0;

  for (int i = 0; i < depth; i++) {
    syms[i].found = dladdr(addrs[i], &syms[i].info);
    if (syms[i].found) {
      if (syms[i].info.dli_saddr && syms[i].info.dli_sname) {
        syms[i].ret_addr_offset =
            (uintptr_t)addrs[i] - (uintptr_t)syms[i].info.dli_saddr;
      } else {
        syms[i].ret_addr_offset =
            (uintptr_t)addrs[i] - (uintptr_t)syms[i].info.dli_fbase;
        syms[i].info.dli_sname = "";
      }

      syms[i].pre_calc_str_size = kFmtSymbolsSize +
                                  strlen(syms[i].info.dli_fname) +
                                  strlen(syms[i].info.dli_sname) +
                                  CalcHexDigits(syms[i].ret_addr_offset) +
                                  CalcHexDigits((uintptr_t)addrs[i]);
    } else {
      syms[i].pre_calc_str_size =
          kFmtNoSymbolsSize + CalcHexDigits((uintptr_t)addrs[i]);
    }

    strs_area_size += syms[i].pre_calc_str_size;
  }

  char **const strs = malloc(depth * sizeof(char *) + strs_area_size);
  if (!strs) {
    goto exit;
  }

  char *str_area = (char *)&strs[depth];

  for (int i = 0; i < depth; i++) {
    strs[i] = str_area;
    if (syms[i].found) {
      snprintf(str_area, syms[i].pre_calc_str_size, kFmtSymbols,
               syms[i].info.dli_fname, syms[i].info.dli_sname,
               syms[i].ret_addr_offset, (uintptr_t)addrs[i]);
    } else {
      snprintf(str_area, syms[i].pre_calc_str_size, kFmtNoSymbols,
               (uintptr_t)addrs[i]);
    }

    str_area += syms[i].pre_calc_str_size;
  }

exit:
  free(syms);
  return strs;
}

void GlibcBt_BacktraceSymbolsFd(void *const *addrs, int depth, int fd) {
  const int new_fd = dup(fd);
  FILE *const file = fdopen(new_fd, "w");
  if (!file) {
    return;
  }

  for (int i = 0; i < depth; i++) {
    Dl_info dl_info;
    if (dladdr(addrs[i], &dl_info)) {
      uintptr_t ret_addr_offset = 0;
      if (dl_info.dli_saddr && dl_info.dli_sname) {
        ret_addr_offset = (uintptr_t)addrs[i] - (uintptr_t)dl_info.dli_saddr;
      } else {
        ret_addr_offset = (uintptr_t)addrs[i] - (uintptr_t)dl_info.dli_fbase;
        dl_info.dli_sname = "";
      }

      fprintf(file, kFmtSymbols, dl_info.dli_fname, dl_info.dli_sname,
              ret_addr_offset, (uintptr_t)addrs[i]);
    } else {
      fprintf(file, kFmtNoSymbols, (uintptr_t)addrs[i]);
    }

    fprintf(file, "\n");
  }

  fflush(file);
  fsync(new_fd);
  fclose(file);
}
