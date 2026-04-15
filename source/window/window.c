/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Creating the window and the rendering context
 *  and running the loop
 */

#include <stdlib.h>

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

bgem_window_handle* bgem_window_createWindow(void)
{
    bgem_window_handle* wh = NULL;
    SDL_Window *window;

    window = SDL_CreateWindow(
        "Project Bluegem",
        HOR_RES / SCL_DWN,
        VER_RES / SCL_DWN,
        SDL_WINDOW_RESIZABLE
    );
    if (!window)
    {
        DEBUG_PRINT("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return NULL;
    }

    wh = (bgem_window_handle*)malloc(sizeof(bgem_window_handle));

    wh->window = window;
    wh->window_ctx = bgem_platform_createContext(wh->window);
    if (!wh->window_ctx) { free(wh); return NULL; }

    return wh;
}
