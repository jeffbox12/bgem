/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "platform/platform_egl.h"
#include "system/config.h"

/**
 * @brief
 * This structure represents the whole window, acting as a handle.
 *
 * The handle is passed to the window and rendering functions to
 * perform rendering operations into its graphics context, and
 * retrieve and change window settings.
 *
 * Must be freed with free() after being assigned.
 */
typedef struct bgem_window_handle {
    SDL_Window* window;                      /**< Used to send and receive window status and set new parameters */
    bgem_platform_windowContext* window_ctx; /**< Used with rendering functions to display the UI */
    bool fullscreen;
} bgem_window_handle;

/**
 * @brief
 * Generates the window and creates the graphics context.
 *
 * By design, all the content in the client area is custom rendered,
 * hence the requirement of the graphics context with the window.
 *
 * @param[in] cfg
 * Takes in the fullscreen status flag and decides if to launch the
 * program in fullscreen (borderless) or windowed mode.
 *
 * @return
 * bgem_window_handle* on success, NULL on failure.
 */
bgem_window_handle* bgem_window_createWindow(bgem_config *cfg);

/**
 * @brief
 * Resize the contents with the new information
 *
 * Extracts window size pixel data, and it uses it for changing the
 * context size.
 *
 * @param[in] wh
 * The window handle to read and apply the new size information.
 *
 * @param[in] event
 * The current SDL event when SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED was sent
 * to read the new client area pixel size data.
 *
 * TODO: Review these descriptions. I didn't feel very confident with that last one
 */
void bgem_window_handleResize(bgem_window_handle *wh, SDL_Event *event);

/**
 * @brief
 * Toggles between borderless or windowed.
 *
 * @param[in] wh
 * Reads the `fullscreen` member and swiches its status.
 */
void bgem_window_toggleFullscreen(bgem_window_handle *wh);

#endif /* WINDOW_H */
