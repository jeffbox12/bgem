/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <SDL3/SDL.h>

/* Basic console debug line printing */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[BGEM] %s:%d: " fmt "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void bgem_debug_init(SDL_Window *window);
void bgem_debug_shutdown(void);
void bgem_debug_toggle(void);
void bgem_debug_newFrame(void);
void bgem_debug_render(void);
int  bgem_debug_isActive(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
