/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "window/window.h"
#include "system/config.h"
#include "core/debug.h"

/**
 * @brief
 * Sets a new client area size to compute the viewport area
 *
 * @param[in] w
 * Width in pixels
 *
 * @param[in] h
 * Height in pixels
 */
void bgem_renderer_setWindowSize(int w, int h);

/**
 * @brief
 * Initialization routine for rendering
 *
 * Loads shaders, creates the FBO, the fullscreen triangle, and the blit
 * shader.
 *
 * @param[in] cfg
 * Reads the interal resolution settings
 */
bgem_result bgem_renderer_init(bgem_config *cfg);

/**
 * @brief
 * Perform the rendering.
 *
 * @param[in] time
 * Time elapsed for time dependent shader animations.
 */
void bgem_renderer_render(float time);

/**
 * @brief
 * Draw the current FBO to the blit shader
 */
void bgem_renderer_present(void);

/**
 * @brief
 * Swap buffers
 *
 * @param[in] ctx
 * Send the buffer swap to the context
 *
 * @return
 * EGL_SUCCESS if successful.
 *
 * TODO: Handle other errors
 */
bgem_result bgem_renderer_swap(bgem_platform_windowContext *ctx);

/**
 * @brief
 * Destroy all the shaders.
 *
 * Call this after `bgem_renderer_init()`
 *
 * TODO: Shaders are created by bgem_renderer_init() because it calls bgem_shader_loadAll().
 * bgem_renderer_destroyAllShaders() calls bgem_shader_destroyAll(). These might not be
 * renderer concerned, and might require restructure and a clear separation. Review this.
 */
void bgem_renderer_destroyAllShaders(void);

#endif /* RENDERER_H */
