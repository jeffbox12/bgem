/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Initializes the notification layer.
 *
 * Currently a transparent stub.
 */
bgem_result bgem_notification_init(void);

/** Releases all resources owned by the notification layer. */
void bgem_notification_destroy(void);

/**
 * @brief
 * Advances the notification layer's per-frame state.
 *
 * @param[in] dt
 * Delta time in seconds since the last frame.
 */
void bgem_notification_update(float dt);

/**
 * @brief
 * Draws the notification layer into whichever FBO is currently bound.
 *
 * The compositor binds and clears the layer's FBO before calling this.
 */
void bgem_notification_render(void);

#endif /* NOTIFICATIONS_H */
