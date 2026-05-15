/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include "system/config.h"
#include "bgem_defaults.h"

bgem_config bgem_config_load(void)
{
    bgem_config cfg = {
        .frame_limit = BGEM_DEFAULT_FRAME_LIMIT,
        .render_height = BGEM_DEFAULT_RENDER_HEIGHT,
        .render_width = BGEM_DEFAULT_RENDER_WIDTH,
        .fullscreen = BGEM_DEFAULT_WINDOW_MODE
    };
    return cfg;
}
