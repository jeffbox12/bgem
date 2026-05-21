/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "window/window.h"
#include "app/app_loop.h"
#include "shader/shader.h"
#include "renderer/renderer.h"
#include "core/frame_limiter.h"
#include "system/config.h"
#include "bgem_defaults.h"
#include "core/timer.h"
#include "core/debug.h"

bgem_result bgem_app_loop(bgem_window_handle *wh, bgem_config *cfg)
{
    int running = 1;
    int status = BGEM_OK;
    SDL_Event event;

    bgem_frame_limiter limiter;
    bgem_frame_limiterInit(&limiter, cfg->frame_limit);

    while (running)
    {
        bgem_frame_limiterBegin(&limiter);

        /* TODO: Move this in a dedicated input module */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_B) {
                    bgem_debug_toggle();
                }
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                bgem_window_handleResize(wh, &event);
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_F) {
                    bgem_window_toggleFullscreen(wh);
                }
            }
        }

        bgem_debug_newFrame();

        bgem_renderer_render((float)limiter.elapsed);

        bgem_renderer_present();

        bgem_debug_render();

        status = bgem_renderer_swap(wh->window_ctx);
        if (status != BGEM_OK) { status = BGEM_ERROR_GPU; running = 0; }

        bgem_frame_limiterEnd(&limiter);

    }

    return status;
}
