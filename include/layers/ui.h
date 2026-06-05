/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef UI_H
#define UI_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Initializes the UI layer.
 *
 * Currently a transparent stub.
 */
bgem_result bgem_ui_init(void);

/** Releases all resources owned by the UI layer. */
void bgem_ui_destroy(void);

/**
 * @brief
 * Advances the UI layer's per-frame state.
 *
 * @param[in] dt
 * Delta time in seconds since the last frame.
 */
void bgem_ui_update(float dt);

/**
 * @brief
 * Draws the UI layer into whichever FBO is currently bound.
 *
 * The compositor binds and clears the layer's FBO before calling this.
 */
void bgem_ui_render(void);

#endif /* UI_H */
