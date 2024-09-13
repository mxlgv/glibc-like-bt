/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Maxim Logaev
 */

#ifndef __TESTS_HELPER_H__
#define __TESTS_HELPER_H__

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr)                                                         \
  if (!(expr)) {                                                             \
    printf("%s:%d: %s: Assertion '%s' failed", __FILE__, __LINE__, __func__, \
           #expr);                                                           \
    exit(1);                                                                 \
  }

#endif /* __TESTS_HELPER_H__ */
