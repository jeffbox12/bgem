/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <GLES3/gl3.h>
#include "window/window.h"
#include "system/config.h"
#include "core/debug.h"

/**
 * @brief
 * Update the letterbox viewport from a new client area size.
 *
 * Call this whenever a window resize event is received.
 *
 * @param[in] w  New width in pixels.
 * @param[in] h  New height in pixels.
 */
void bgem_renderer_setWindowSize(int w, int h);

/**
 * @brief
 * Initialize the renderer.
 *
 * Compiles the blit shader and allocates the fullscreen triangle VBO.
 * Must be called after bgem_shader_loadAll().
 *
 * @param[in] cfg  Reads the internal resolution settings.
 */
bgem_result bgem_renderer_init(bgem_config *cfg);

/**
 * @brief
 * Blit the compositor result to the window with letterboxing.
 *
 * Clears the full window to black, then draws tex into the fitted
 * letterbox viewport. Call bgem_renderer_swap() afterwards.
 *
 * @param[in] tex  GL texture handle returned by bgem_compositor_composite().
 */
void bgem_renderer_present(GLuint tex);

/**
 * @brief
 * Swap the EGL back buffer to screen.
 *
 * @param[in] ctx  The platform window context.
 * @return BGEM_OK on success, BGEM_ERROR_GPU on EGL failure.
 */
bgem_result bgem_renderer_swap(bgem_platform_windowContext *ctx);

/**
 * @brief
 * Destroy all compiled shaders.
 *
 * TODO: bgem_shader_loadAll() is currently called from app.c rather than
 * here. bgem_renderer_destroyAllShaders() and its counterpart may belong
 * at the app level once the shader lifecycle is clarified.
 */
void bgem_renderer_destroyAllShaders(void);

#endif /* RENDERER_H */
