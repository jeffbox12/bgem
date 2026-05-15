/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Multiplatform abstraction
 */

#ifndef PLATFORM_EGL_H
#define PLATFORM_EGL_H

#include <SDL3/SDL.h>

typedef struct bgem_platform_windowContext bgem_platform_windowContext;

/* Create graphics context attached to SDL window */

/**
 * @brief
 * Create an EGL context and attach it to the provided window.
 *
 * @param[in] window
 * The window created by SDL to attatch the EGL context.
 *
 * @return
 * Returns a struct with all the members initialized, ready to be controlled
 * by future EGL calls. NULL if a failure occurred.
 */
bgem_platform_windowContext* bgem_platform_createContext(SDL_Window *window);

/**
 * @brief
 * Call EGL's swap buffer function.
 *
 * @param[in] platformContext
 * The valid window context structure to read from and execute the EGL's swap
 * buffer function.
 */
void bgem_platform_swapBuffers(bgem_platform_windowContext *platformContext);

/**
 * @brief
 * Resize the context created by the Wayland compositor
 *
 * @param[in] platformContext
 * If the window context has a non-null wl_win member, it will be read by the
 * Wayland specific function and apply it to the appropriate context to resize.
 *
 * @param[in] w
 * Width in pixels
 *
 * @param[in] h
 * Height in pixels
 */
void bgem_platform_waylandResizeSurface(bgem_platform_windowContext *platformContext, int w, int h);

/**
 * @brief
 * Destroy the context.
 *
 * Must be called after a successful bgem_platform_createContext()
 *
 * @param[in] platformContext
 * The struct to clear up.
 */
void bgem_platform_destroyContext(bgem_platform_windowContext *platformContext);

#endif /* PLATFORM_EGL_H */
