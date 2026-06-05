/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef BLEND_H
#define BLEND_H

#include <GLES3/gl3.h>
#include "compositor/layer.h" /* bgem_layer_id, BGEM_LAYER_COUNT */
#include "core/debug.h"       /* bgem_result */

/**
 * @brief
 * Initialize the blend pass.
 *
 * Fetches the blend shader from the catalog, queries all sampler uniform
 * locations, and allocates the fullscreen triangle VBO.
 *
 * Must be called after bgem_shader_loadAll().
 */
bgem_result bgem_blend_init(void);

/**
 * @brief
 * Release all resources owned by the blend pass.
 */
void bgem_blend_shutdown(void);

/**
 * @brief
 * Execute the blend pass.
 *
 * Binds result_fbo, samples all BGEM_LAYER_COUNT layer textures in
 * z-order, composites them using straight-alpha blending, and writes
 * the result. Unbinds the framebuffer before returning.
 *
 * @param[in] layer_tex   Texture handles indexed by bgem_layer_id.
 * @param[in] result_fbo  Framebuffer to render the composited output into.
 * @param[in] w           Viewport width in pixels.
 * @param[in] h           Viewport height in pixels.
 */
void bgem_blend_execute(GLuint layer_tex[BGEM_LAYER_COUNT],
                        GLuint result_fbo, int w, int h);

#endif /* BLEND_H */
