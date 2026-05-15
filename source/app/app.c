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

    SDL_GetWindowSizeInPixels(wh->window, &w, &h);
    bgem_renderer_setWindowSize(w, h);
    bgem_renderer_init(cfg);
    bgem_debug_init(wh->window);

    bgem_app_loop(wh, cfg);

    bgem_debug_shutdown();
    bgem_renderer_destroyAllShaders();
    free(wh);
    return EXIT_SUCCESS;
}
