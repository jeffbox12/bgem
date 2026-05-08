/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <SDL3/SDL.h>

void bgem_platform_windowBringForward(void)
{
    // Open a message box for now. Wayland does not support sending window mode commands
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                             "Project Bluegem",
                             "Project Bluegem is already opened.",
                             NULL);
}
