/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "platform/platform_window.h"

typedef struct bgem_window_handle {
    SDL_Window* window;
    bgem_platform_windowContext* window_ctx;
} bgem_window_handle;

bgem_window_handle* bgem_window_createWindow(void);
int bgem_window_createContext(bgem_window_handle* wh);

#endif /* WINDOW_H */
