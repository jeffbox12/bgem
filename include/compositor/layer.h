/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef LAYER_H
#define LAYER_H

/**
 * @brief
 * Z-ordered identifiers for each compositor layer and FBO array index.
 *
 * Values are assigned bottom-to-top: index 0 is furthest from the viewer,
 * BGEM_LAYER_COUNT - 1 is closest. The blend pass samples layers in this
 * order, so higher indices composite over lower ones.
 *
 * This set is fixed by design — it is part of the shell's architecture,
 * not an open registration point. Adding or removing a layer means
 * editing compositor.c directly.
 */
typedef enum {
    BGEM_LAYER_BACKGROUND   = 0,
    BGEM_LAYER_APPLICATION  = 1,
    BGEM_LAYER_UI           = 2,
    BGEM_LAYER_OVERLAY      = 3,
    BGEM_LAYER_NOTIFICATION = 4,
    BGEM_LAYER_COUNT        = 5
} bgem_layer_id;

#endif /* LAYER_H */
