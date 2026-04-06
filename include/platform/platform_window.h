/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include <SDL3/SDL.h>

typedef struct PlatformContext PlatformContext;

/* Create graphics context attached to SDL window */
PlatformContext* bgem_platform_createContext(SDL_Window *window);

/* Swap buffers */
void bgem_platform_swapBuffers(PlatformContext *ctx);

/* Destroy graphics context */
void bgem_platform_destroyContext(PlatformContext *ctx);

#endif //PLATFORM_WINDOW_H
