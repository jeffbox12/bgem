/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <SDL3/SDL.h>
#include <GLES3/gl3.h>

#include "renderer/renderer.h"
#include "shader/shader.h"
#include "platform/platform_egl.h"
#include "system/config.h"
#include "bgem_defaults.h"
#include "core/debug.h"

static GLuint blit_program;
static GLuint blit_vbo;
static GLint  blit_tex_loc;
static GLint  blit_pos_loc;

/* Internal renderer resolution */
static int internal_w = 0;
static int internal_h = 0;

/* Cached letterbox viewport */
static struct {
    int x, y, w, h;
    int window_w, window_h;
} viewport;

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

bgem_result bgem_renderer_init(bgem_config *cfg)
{
    internal_w = cfg->render_width;
    internal_h = cfg->render_height;

    blit_program = bgem_shader_get("blit");
    if (!blit_program) { DEBUG_PRINT("Shader 'blit' not found"); return BGEM_ERROR_IO; }
    blit_pos_loc = glGetAttribLocation(blit_program,  "aPos");
    blit_tex_loc = glGetUniformLocation(blit_program, "uTex");

    float verts[] = { -1.0f, -1.0f,  3.0f, -1.0f,  -1.0f,  3.0f };
    glGenBuffers(1, &blit_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return BGEM_OK;
}

void bgem_renderer_present(GLuint tex)
{
    /* Clear the full window (fills letterbox bars with black) */
    glViewport(0, 0, viewport.window_w, viewport.window_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Blit the compositor result into the fitted letterbox rect */
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glUseProgram(blit_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(blit_tex_loc, 0);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glEnableVertexAttribArray(blit_pos_loc);
    glVertexAttribPointer(blit_pos_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(blit_pos_loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bgem_result bgem_renderer_swap(bgem_platform_windowContext *ctx)
{
    if (bgem_platform_swapBuffers(ctx) != EGL_SUCCESS)
        return BGEM_ERROR_GPU;
    else
        return BGEM_OK;
}

void bgem_renderer_destroyAllShaders(void)
{
    bgem_shader_destroyAll();
}
