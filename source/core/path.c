/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <SDL3/SDL.h>

#include "core/path.h"

bool bgem_path_relativeToFull(char *out, size_t size, const char *relative_path)
{
    const char* base = SDL_GetBasePath();
    if (!base) return false;
    int written = snprintf(out, size, "%s%s", base, relative_path);
    return written > 0 && (size_t)written < size;
}
