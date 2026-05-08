/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "system/system.h"
#include "app/app.h"
#include "core/debug.h"
#include "bgem_version.h"
#include "platform/platform_init.h"

#include <SDL3/SDL.h>

int bgem_system_start(void)
{
    /* Lock instance */
    if(bgem_platform_instanceLock() != BGEM_INSTANCE_LOCK_OK) return EXIT_FAILURE;

    DEBUG_PRINT("%s", BGEM_VERSION_STRING);
    DEBUG_PRINT("Hello Bluegem!");

    SDL_SetAppMetadata("Project Bluegem", BGEM_VERSION_STRING, "com.bgem.bgem");


    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0)
    {
        DEBUG_PRINT("SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(bgem_app_init()) return EXIT_FAILURE;

    /* Unlock instance once closing the program */
    bgem_platform_instanceUnlock();

    return EXIT_SUCCESS;
}
