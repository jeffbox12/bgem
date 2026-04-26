/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Multiplatform abstraction
 */

#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include <SDL3/SDL.h>

typedef struct bgem_platform_windowContext bgem_platform_windowContext;

/* Create graphics context attached to SDL window */
bgem_platform_windowContext* bgem_platform_createContext(SDL_Window *window);
void bgem_platform_swapBuffers(bgem_platform_windowContext *platformContext);
void bgem_platform_destroyContext(bgem_platform_windowContext *platformContext);

#endif //PLATFORM_WINDOW_H
