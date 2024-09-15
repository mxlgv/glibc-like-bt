/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#ifndef _GLIBCBT_H_
#define _GLIBCBT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function getting the frame address of the current function:
void *GlibcBt_GetFrameAddr(void);

// Fully compatible with backtrace functions from the GNU C library:
int GlibcBt_Backtrace(void **addrs, int depth);
char **GlibcBt_BacktraceSymbols(void **addrs, int depth);
void GlibcBt_BacktraceSymbolsFd(void **addrs, int depth, int fd);

#ifdef __cplusplus
}
#endif

#endif /*_GLIBCBT_H_*/
