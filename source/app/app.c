/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "renderer/renderer.h"
#include "compositor/compositor.h"
#include "shader/shader.h"
#include "window/window.h"
#include "input/input.h"
#include "app/app.h"
#include "app/app_loop.h"
#include "core/debug.h"

int bgem_app_run(bgem_config *cfg)
{
    int w, h;
    bgem_window_handle *wh;

    wh = bgem_window_createWindow(cfg);
    if (!wh) return EXIT_FAILURE;

    if (!SDL_GetWindowSizeInPixels(wh->window, &w, &h))
    {
        DEBUG_PRINT("SDL_GetWindowSizeInPixels failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    bgem_renderer_setWindowSize(w, h);

    if (bgem_shader_loadAll())        { DEBUG_PRINT("Failed loading shaders");              return EXIT_FAILURE; }
    if (bgem_renderer_init(cfg))      { DEBUG_PRINT("Failed to initialize the renderer");   return EXIT_FAILURE; }
    if (bgem_compositor_init(cfg->render_width, cfg->render_height))
                                      { DEBUG_PRINT("Failed to initialize the compositor"); return EXIT_FAILURE; }
    if (bgem_input_init())            { DEBUG_PRINT("Failed to initialize input");          return EXIT_FAILURE; }

    BGEM_DEBUG_INIT(wh->window);

    if (bgem_app_loop(wh, cfg)) { DEBUG_PRINT("Fatal error in app loop"); } /* TODO: Return failure if an error occurs */

    BGEM_DEBUG_SHUTDOWN();
    bgem_input_shutdown();
    bgem_compositor_shutdown();
    bgem_renderer_destroyAllShaders();
    free(wh);
    return EXIT_SUCCESS;
}
