/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "renderer/renderer.h"
#include "renderer/loop.h"
#include "window/window.h"
#include "app/app.h"
#include "core/debug.h"

int bgem_app_init(void)
{
    bgem_renderer_loop(bgem_window_createWindow());
    return EXIT_SUCCESS;
}
