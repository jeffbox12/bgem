/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include <SDL3/SDL.h>

#include "window/window.h"
#include "platform/platform_egl.h"
#include "renderer/renderer.h"
#include "core/debug.h"
#include "bgem.h"

// Window resoluton (not internal renderer)
#define HOR_RES 1920
#define VER_RES 1080
#define SCL_DWN 2 // Scale Down using division

bgem_window_handle* bgem_window_createWindow(bgem_config *cfg)
{
    bgem_window_handle* wh = NULL;
    SDL_Window *window;

    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    if(display == 0) { DEBUG_PRINT("SDL_GetPrimaryDisplay error: %s", SDL_GetError()); return NULL; }
    float scale = SDL_GetDisplayContentScale(display);
    if(scale <= 0.0f) { scale = 1.0f; DEBUG_PRINT("SDL_GetPrimaryDisplay error: %s", SDL_GetError()); return NULL; }
    int win_w = (int)((HOR_RES / SCL_DWN) * scale);
    int win_h = (int)((VER_RES / SCL_DWN) * scale);

    window = SDL_CreateWindow(
        BGEM_APP_NAME,
        win_w,
        win_h,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!window)
    {
        DEBUG_PRINT("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return NULL;
    }

    wh = (bgem_window_handle*)malloc(sizeof(bgem_window_handle));

    if (!wh) { SDL_DestroyWindow(window); return NULL; }
    wh->window = window;

    wh->window_ctx = bgem_platform_createContext(wh->window);
    if (!wh->window_ctx) { SDL_DestroyWindow(window); free(wh); return NULL; }

    wh->fullscreen = cfg->fullscreen;
    if(!SDL_SetWindowFullscreen(wh->window, wh->fullscreen)) { DEBUG_PRINT("SDL_SetWindowFullscreen error: %s", SDL_GetError()); }

    return wh;
}

void bgem_window_handleResize(bgem_window_handle *wh, SDL_Event *event)
{
    int w, h;

#if defined(__linux__)
    w = event->window.data1;
    h = event->window.data2;
    bgem_platform_waylandResizeSurface(wh->window_ctx, w, h);
#elif defined(__APPLE__)
    (void)event; /* Silence warning */
    if(!SDL_GetWindowSizeInPixels(wh->window, &w, &h)) { DEBUG_PRINT("SDL_GetWindowSizeInPixels error: %s", SDL_GetError()); }
#elif defined(_WIN32)
    w = event->window.data1;
    h = event->window.data2;
#endif

    bgem_renderer_setWindowSize(w, h);
}

void bgem_window_toggleFullscreen(bgem_window_handle *wh)
{
    wh->fullscreen = !wh->fullscreen;
    if(!SDL_SetWindowFullscreen(wh->window, wh->fullscreen)) { DEBUG_PRINT("SDL_SetWindowFullscreen error: %s", SDL_GetError()); }
}
