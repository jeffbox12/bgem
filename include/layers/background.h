/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Compiles the testshader, allocates the fullscreen-triangle VBO, and
 * resets the time accumulator.
 *
 * Must be called after bgem_shader_loadAll().
 */
bgem_result bgem_background_init(void);

/**
 * @brief
 * Releases all GL resources owned by the background layer.
 */
void bgem_background_destroy(void);

/**
 * @brief
 * Advances the background's internal time accumulator.
 *
 * @param[in] dt
 * Delta time in seconds since the last frame.
 */
void bgem_background_update(float dt);

/**
 * @brief
 * Draws the background into whichever FBO is currently bound.
 *
 * The compositor binds and clears the layer's FBO before calling this.
 * Does not bind or unbind any framebuffer itself.
 */
void bgem_background_render(void);

#endif /* BACKGROUND_H */
