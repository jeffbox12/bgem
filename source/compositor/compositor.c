/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <GLES3/gl3.h>

#include "compositor/compositor.h"
#include "compositor/blend.h"
#include "compositor/layer.h"
#include "renderer/fbo.h"

#include "layers/background.h"
#include "layers/application.h"
#include "layers/ui.h"
#include "layers/overlay.h"
#include "layers/notifications.h"

#include "core/debug.h"

/* Per-layer FBOs, indexed by bgem_layer_id */
static GLuint s_layer_fbo[BGEM_LAYER_COUNT];
static GLuint s_layer_tex[BGEM_LAYER_COUNT];

/* Compositor result FBO */
static GLuint s_result_fbo;
static GLuint s_result_tex;

/* Internal resolution */
static int s_width;
static int s_height;

/**
 * @brief
 * Binds a layer's FBO and clears it to transparent, ready for that
 * layer's render() to draw into.
 */
static void prepareLayer(bgem_layer_id id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_layer_fbo[id]);
    glViewport(0, 0, s_width, s_height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

bgem_result bgem_compositor_init(int width, int height)
{
    bgem_result status;

    s_width  = width;
    s_height = height;

    /* One FBO per layer */
    for (int i = 0; i < BGEM_LAYER_COUNT; i++)
    {
        status = bgem_renderer_fboCreate(width, height, &s_layer_fbo[i], &s_layer_tex[i]);
        if (status != BGEM_OK)
        {
            DEBUG_PRINT("bgem_compositor_init: failed to create layer FBO %d", i);
            return status;
        }
    }

    /* Result FBO for the blend pass output */
    status = bgem_renderer_fboCreate(width, height, &s_result_fbo, &s_result_tex);
    if (status != BGEM_OK)
    {
        DEBUG_PRINT("bgem_compositor_init: failed to create result FBO");
        return status;
    }

    status = bgem_blend_init();
    if (status != BGEM_OK) return status;

    /*
     * NOTE: If the order changes, remember to reflect the change in the
     *       blend fragment shader.
     */
    status = bgem_background_init();
    if (status != BGEM_OK) { DEBUG_PRINT("bgem_compositor_init: background init failed");   return status; }

    status = bgem_application_init();
    if (status != BGEM_OK) { DEBUG_PRINT("bgem_compositor_init: application init failed");  return status; }

    status = bgem_ui_init();
    if (status != BGEM_OK) { DEBUG_PRINT("bgem_compositor_init: ui init failed");           return status; }

    status = bgem_overlay_init();
    if (status != BGEM_OK) { DEBUG_PRINT("bgem_compositor_init: overlay init failed");      return status; }

    status = bgem_notification_init();
    if (status != BGEM_OK) { DEBUG_PRINT("bgem_compositor_init: notification init failed"); return status; }

    return BGEM_OK;
}

void bgem_compositor_shutdown(void)
{
    /* Reverse of init order */
    bgem_notification_destroy();
    bgem_overlay_destroy();
    bgem_ui_destroy();
    bgem_application_destroy();
    bgem_background_destroy();

    bgem_blend_shutdown();

    for (int i = 0; i < BGEM_LAYER_COUNT; i++)
    {
        glDeleteFramebuffers(1, &s_layer_fbo[i]);
        glDeleteTextures(1, &s_layer_tex[i]);
    }

    glDeleteFramebuffers(1, &s_result_fbo);
    glDeleteTextures(1, &s_result_tex);
}

GLuint bgem_compositor_composite(float dt)
{
    prepareLayer(BGEM_LAYER_BACKGROUND);
    bgem_background_update(dt);
    bgem_background_render();

    prepareLayer(BGEM_LAYER_APPLICATION);
    bgem_application_update(dt);
    bgem_application_render();

    prepareLayer(BGEM_LAYER_UI);
    bgem_ui_update(dt);
    bgem_ui_render();

    prepareLayer(BGEM_LAYER_OVERLAY);
    bgem_overlay_update(dt);
    bgem_overlay_render();

    prepareLayer(BGEM_LAYER_NOTIFICATION);
    bgem_notification_update(dt);
    bgem_notification_render();

    bgem_blend_execute(s_layer_tex, s_result_fbo, s_width, s_height);

    return s_result_tex;
}
