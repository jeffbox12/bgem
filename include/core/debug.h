/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

/* Basic console debug line printing */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[BGEM] %s:%d: " fmt "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

void bgem_debug_stud(void);

#endif /* DEBUG_H */
