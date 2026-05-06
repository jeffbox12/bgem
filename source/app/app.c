/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "renderer/renderer.h"
#include "renderer/render_loop.h"
#include "window/window.h"
#include "app/app.h"
#include "core/debug.h"

int bgem_app_init(void)
{
    bgem_window_handle* wh = bgem_window_createWindow();
    if(!wh) return EXIT_FAILURE;

    bgem_renderer_loop(wh);
    free(wh);
    return EXIT_SUCCESS;
}
