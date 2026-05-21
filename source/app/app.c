/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "renderer/renderer.h"
#include "window/window.h"
#include "app/app.h"
#include "app/app_loop.h"
#include "core/debug.h"

int bgem_app_run(bgem_config *cfg)
{
    int w, h;
    bgem_window_handle* wh;

    wh = bgem_window_createWindow(cfg);
    if (!wh) return EXIT_FAILURE;

    if(!SDL_GetWindowSizeInPixels(wh->window, &w, &h)) { DEBUG_PRINT("SDL_GetWindowSizeInPixels failed with %s", SDL_GetError()); return EXIT_FAILURE; }
    bgem_renderer_setWindowSize(w, h);
    if(bgem_renderer_init(cfg)) { DEBUG_PRINT("Failed to initialize the renderer."); return EXIT_FAILURE; }
    bgem_debug_init(wh->window);

    if(bgem_app_loop(wh, cfg)) { DEBUG_PRINT("Fatal error occurred!"); return EXIT_FAILURE; }

    bgem_debug_shutdown();
    bgem_renderer_destroyAllShaders();
    free(wh);
    return EXIT_SUCCESS;
}
