/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>

#include "window/window.h"
#include "input/input.h"
#include "input/input_debug.h"
#include "app/app_loop.h"
#include "compositor/compositor.h"
#include "renderer/renderer.h"
#include "core/frame_limiter.h"
#include "core/debug.h"
#include "system/config.h"

bgem_result bgem_app_loop(bgem_window_handle *wh, bgem_config *cfg)
{
    int running = 1;
    int status  = BGEM_OK;
    GLuint fbo_texture = GL_NONE;

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

        fbo_texture = bgem_compositor_composite(limiter.dt_ms / 1000.0f); /* Explicitly converts ms to secs */
        if (fbo_texture == GL_NONE) { status = BGEM_ERROR_GPU; running = 0; }
        bgem_renderer_present(fbo_texture);

        BGEM_DEBUG_RENDER();

        status = bgem_renderer_swap(wh->window_ctx);
        if (status != BGEM_OK) { status = BGEM_ERROR_GPU; running = 0; }

        bgem_frame_limiterEnd(&limiter);
    }

    return status;
}
