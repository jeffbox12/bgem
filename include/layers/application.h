/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Initializes the application layer.
 *
 * Currently a transparent stub.
 */
bgem_result bgem_application_init(void);

/** Releases all resources owned by the application layer. */
void bgem_application_destroy(void);

/**
 * @brief
 * Advances the application layer's per-frame state.
 *
 * @param[in] dt
 * Delta time in seconds since the last frame.
 */
void bgem_application_update(float dt);

/**
 * @brief
 * Draws the application layer into whichever FBO is currently bound.
 *
 * The compositor binds and clears the layer's FBO before calling this.
 */
void bgem_application_render(void);

#endif /* APPLICATION_H */
