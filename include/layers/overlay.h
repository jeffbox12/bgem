/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef OVERLAY_H
#define OVERLAY_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Initializes the quick-settings overlay layer.
 *
 * Currently a transparent stub.
 */
bgem_result bgem_overlay_init(void);

/** Releases all resources owned by the overlay layer. */
void bgem_overlay_destroy(void);

/**
 * @brief
 * Advances the overlay layer's per-frame state.
 *
 * @param[in] dt
 * Delta time in seconds since the last frame.
 */
void bgem_overlay_update(float dt);

/**
 * @brief
 * Draws the overlay layer into whichever FBO is currently bound.
 *
 * The compositor binds and clears the layer's FBO before calling this.
 */
void bgem_overlay_render(void);

#endif /* OVERLAY_H */
