/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <SDL3/SDL.h>

#include "core/path.h"
#include "core/debug.h"

bgem_result bgem_path_relativeToFull(char *out, size_t size, const char *relative_path)
{
    const char* base = SDL_GetBasePath();
    if (!base) { DEBUG_PRINT("SDL_GetBasePath error: %s", SDL_GetError()); return BGEM_ERROR_IO; }
    int written = snprintf(out, size, "%s%s", base, relative_path);
    if(written > 0 && (size_t)written < size)
    {
        return BGEM_OK;
    }
    else
    {
        DEBUG_PRINT("Buffer overflow!");
        return BGEM_ERROR_IO;
    }
}
