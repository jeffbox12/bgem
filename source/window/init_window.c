/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Creating the window and the rendering context
 *  and running the loop
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

#include "window/init_window.h"
#include "platform/platform_window.h"
#include "renderer/shader.h"
#include "renderer/renderer.h"
#include "core/debug.h"

// Window resoluton (not internal renderer)
#define HOR_RES 1920
#define VER_RES 1080
#define SCL_DWN 2 // Scale Down using division

static double get_time_seconds()
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

int bgem_system_start(void)
{
    SDL_Window *window;
    SDL_Event event;
    PlatformContext *platformCtx;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        DEBUG_PRINT("SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(
        "Project Bluegem",
        HOR_RES / SCL_DWN,
        VER_RES / SCL_DWN,
        SDL_WINDOW_RESIZABLE
    );
    if (!window)
    {
        DEBUG_PRINT("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    platformCtx = bgem_platform_createContext(window);
    if (!platformCtx) return EXIT_FAILURE;

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

        bgem_platform_swapBuffers(platformCtx);

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time)
            SDL_Delay(frame_delay - frame_time);
    }

    bgem_platform_destroyContext(platformCtx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
