/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "platform/platform_window.h"

typedef enum bgem_window_mode {
    BGEM_DEFAULT,
    BGEM_WINDOWED,
    BGEM_FULLSCREEN
} bgem_window_mode;

typedef struct bgem_window_handle {
    SDL_Window* window;
    bgem_platform_windowContext* window_ctx;
} bgem_window_handle;

bgem_window_handle* bgem_window_createWindow(bgem_window_mode wm);
int bgem_window_createContext(bgem_window_handle* wh);

#endif /* WINDOW_H */
