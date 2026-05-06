/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "system/system.h"
#include "app/app.h"
#include "core/debug.h"

#include <SDL3/SDL.h>

int bgem_system_start(void)
{
    DEBUG_PRINT("Hello Bluegem!");

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0)
    {
        DEBUG_PRINT("SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(bgem_app_init()) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
