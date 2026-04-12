/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef INIT_WINDOW_H
#define INIT_WINDOW_H

#include "platform/platform_window.h"

typedef struct bgem_createWindow {
    SDL_Window* window;
    bgem_platform_windowContext* window_ctx;
    int status;
} bgem_createWindow;

bgem_createWindow* bgem_window_createWindow(void);
int bgem_window_createContext(bgem_createWindow* cw);

#endif /* INIT_WINDOW_H */
