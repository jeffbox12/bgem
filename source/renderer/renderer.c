/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include <SDL3/SDL.h>
#include <GLES3/gl3.h>

#include "renderer/renderer.h"
#include "shader/shader.h"
#include "platform/platform_egl.h"
#include "system/config.h"
#include "bgem_defaults.h"
#include "core/debug.h"

static GLuint program;
static GLuint vbo;
static GLint  timeLocation;
static GLint  posLocation;

static GLuint fbo;
static GLuint fbo_texture;

static GLuint blit_program;
static GLuint blit_vbo;
static GLint  blit_texLocation;
static GLint  blit_posLocation;

/* Internal renderer resolution */
static int internal_w = 0;
static int internal_h = 0;

/* Cached letterbox viewport */
static struct {
    int x, y, w, h;
    int window_w, window_h; /* updated via bgem_renderer_setWindowSize() */
} viewport;

void bgem_renderer_initInternalResolution(bgem_config *cfg)
{
    internal_w = cfg->render_width;
    internal_h = cfg->render_height;
}

void bgem_renderer_setWindowSize(int w, int h)
{
    viewport.window_w = w;
    viewport.window_h = h;

    float scale_x = (float)w / (float)internal_w;
    float scale_y = (float)h / (float)internal_h;
    float scale   = scale_x < scale_y ? scale_x : scale_y; /* fit, no crop */

    viewport.w = (int)(internal_w * scale);
    viewport.h = (int)(internal_h * scale);
    viewport.x = (w - viewport.w) / 2;
    viewport.y = (h - viewport.h) / 2;
}

void bgem_renderer_init(bgem_config *cfg)
{
    bgem_renderer_initInternalResolution(cfg);
    bgem_shader_loadAll();

    /* Creste offscreen FBO */
    /* Color texture at internal resolution */
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 internal_w, internal_h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* FBO */
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fbo_texture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        DEBUG_PRINT("FBO incomplete: 0x%x\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Create `testshader` shader */
    /* TODO: This should not be here */
    program = bgem_shader_get("testshader");
    posLocation = glGetAttribLocation(program, "aPos");
    timeLocation = glGetUniformLocation(program, "uTime");

    /* Create `blit` shader */
    blit_program = bgem_shader_get("blit");
    blit_posLocation = glGetAttribLocation(blit_program, "aPos");
    blit_texLocation = glGetUniformLocation(blit_program, "uTex");

    /* Define vertices for fullscreen triangle */
    float vertices[] = { -1.0f, -1.0f,  3.0f, -1.0f,  -1.0f,  3.0f };

    /* Store vertices in a buffer */
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* Reuse the same fullscreen triangle geometry */
    glGenBuffers(1, &blit_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void bgem_renderer_render(float time)
{
    /* Render scene into FBO at internal resolution */
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, internal_w, internal_h);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glUniform1f(timeLocation, time);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bgem_renderer_present(void)
{
    /* Clear the full window (shows letterbox bars in clear color) */
    glViewport(0, 0, viewport.window_w, viewport.window_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Blit FBO texture into the fitted rect */
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glUseProgram(blit_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glUniform1i(blit_texLocation, 0);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glEnableVertexAttribArray(blit_posLocation);
    glVertexAttribPointer(blit_posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void bgem_renderer_swap(bgem_platform_windowContext *ctx)
{
    bgem_platform_swapBuffers(ctx);
}

void bgem_renderer_destroyAllShaders(void)
{
    bgem_shader_destroyAll();
}
