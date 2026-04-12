/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(__APPLE__) || defined(__linux__)
#include <time.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif
#include <SDL3/SDL.h>

#include "window/window.h"
#include "platform/platform_window.h"
#include "renderer/render_loop.h"
#include "renderer/shader.h"
#include "renderer/renderer.h"
#include "core/debug.h"

static double get_time_seconds(void)
{
#if defined(_WIN32)
    LARGE_INTEGER count;
    static LARGE_INTEGER frequency;
    static int initialized = 0;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        initialized = 1;
    }

    QueryPerformanceCounter(&count);

    return (double)count.QuadPart / (double)frequency.QuadPart;

#elif defined(__APPLE__) || defined(__linux__)
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec / 1000000000.0;

#endif
}

int bgem_renderer_loop(bgem_window_handle *wh)
{
    int running = 1;
    SDL_Event event;

    bgem_renderer_init();

    const Uint64 target_fps = 60;
    const Uint64 frame_delay = 1000 / target_fps;

    Uint64 frame_start;
    Uint64 frame_time;

    double startTime = get_time_seconds();

    while (running)
    {
        frame_start = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                running = 0;  // Exit on click
            }
        }
        double current = get_time_seconds();
        float time = (float)(current - startTime);

        bgem_renderer_render(time);

        bgem_renderer_present(wh->window_ctx);

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time)
            SDL_Delay((Uint32)(frame_delay - frame_time));
    }

    bgem_platform_destroyContext(wh->window_ctx);
    SDL_DestroyWindow(wh->window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
