/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <SDL3/SDL.h>

#include "window/window.h"
#include "input/input.h"
#include "input/input_debug.h"
#include "app/app_loop.h"
#include "renderer/renderer.h"
#include "core/frame_limiter.h"
#include "core/debug.h"
#include "system/config.h"

bgem_result bgem_app_loop(bgem_window_handle *wh, bgem_config *cfg)
{
    int running = 1;
    int status  = BGEM_OK;

    bgem_frame_limiter limiter;
    bgem_frame_limiterInit(&limiter, cfg->frame_limit);

    while (running)
    {
        bgem_frame_limiterBegin(&limiter);

        bgem_input_update(wh);

        if (bgem_input_quitRequested()) { running = 0; }

        if (bgem_input_actionJustPressed(BGEM_ACTION_FULLSCREEN_TOGGLE)) { bgem_window_toggleFullscreen(wh); }

        BGEM_INPUT_DEBUG();

        BGEM_DEBUG_NEWFRAME();

        bgem_renderer_render((float)limiter.elapsed);
        bgem_renderer_present();

        BGEM_DEBUG_RENDER();

        status = bgem_renderer_swap(wh->window_ctx);
        if (status != BGEM_OK) { status = BGEM_ERROR_GPU; running = 0; }

        bgem_frame_limiterEnd(&limiter);
    }

    return status;
}
