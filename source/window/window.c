/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Creating the window and the rendering context
 *  and running the loop
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(__APPLE__) || defined(__linux__)
#include <time.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif
#include <SDL3/SDL.h>

#include "window/window.h"
#include "platform/platform_window.h"
#include "renderer/shader.h"
#include "renderer/renderer.h"
#include "core/debug.h"

// Window resoluton (not internal renderer)
#define HOR_RES 1920
#define VER_RES 1080
#define SCL_DWN 2 // Scale Down using division

bgem_createWindow* bgem_window_createWindow(void)
{
    bgem_createWindow* cw = {0};
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        DEBUG_PRINT("SDL_Init failed: %s\n", SDL_GetError());
        cw->status = EXIT_FAILURE;
        return cw;
    }

    window = SDL_CreateWindow(
        "Project Bluegem",
        HOR_RES / SCL_DWN,
        VER_RES / SCL_DWN,
        SDL_WINDOW_RESIZABLE
    );
    if (!window)
    {
        DEBUG_PRINT("SDL_CreateWindow failed: %s\n", SDL_GetError());
        cw->status = EXIT_FAILURE;
        return cw;
    }

    cw = (bgem_createWindow*)malloc(sizeof(bgem_createWindow));

    cw->window = window;
    cw->window_ctx = bgem_platform_createContext(cw->window);
    cw->status = EXIT_SUCCESS;

    return cw;
}
