/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <SDL3/SDL.h>

/* Basic console debug line printing */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[BGEM] %s:%d: " fmt "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Initialize the Imgui debug UI
 *
 * @param[in] window
 * The window created by SDL. Must be the main Bluegem window
 *
 */
void bgem_debug_init(SDL_Window *window);

/**
 * @brief
 * Destroy the Imgui context
 *
 * Must be called after bgem_debug_init()
 */
void bgem_debug_shutdown(void);

/**
 * @brief
 * On/Off switch to show the UI or not
 */
void bgem_debug_toggle(void);

/**
 * @brief
 * Generate one frame of the ImGui interface
 *
 * It has to be called in a per tick/frame loop.
 *
 * TODO: The user interface is created inside here,
 *       probably has to be moved somewhere else.
 */
void bgem_debug_newFrame(void);

/**
 * @brief
 * Render the debug UI
 */
void bgem_debug_render(void);

/**
 * @brief
 * Returns the status set by bgem_debug_toggle()
 *
 * @return
 * TRUE if enabled. FALSE if disabled.
 */
int  bgem_debug_isActive(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
