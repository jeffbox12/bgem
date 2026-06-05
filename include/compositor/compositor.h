/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <GLES3/gl3.h>
#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Initialize the compositor.
 *
 * Allocates one FBO per layer plus one result FBO, initialises the blend
 * pass, and calls init() on every registered layer.
 *
 * Must be called after bgem_shader_loadAll().
 *
 * @param[in] width   Internal render width in pixels.
 * @param[in] height  Internal render height in pixels.
 */
bgem_result bgem_compositor_init(int width, int height);

/**
 * @brief
 * Shut down the compositor and release all GPU resources.
 *
 * Calls destroy() on every layer, then deletes all FBOs and textures.
 */
void bgem_compositor_shutdown(void);

/**
 * @brief
 * Update and composite all enabled layers into the result texture.
 *
 * For each enabled layer, calls update() then renders it into its private
 * FBO. Runs the blend pass to merge all layer textures into the result FBO.
 *
 * @param[in] dt  Frame delta time in seconds.
 *
 * @return  GL texture handle of the composited result.
 *          Pass this directly to bgem_renderer_present().
 */
GLuint bgem_compositor_composite(float dt);

#endif /* COMPOSITOR_H */
