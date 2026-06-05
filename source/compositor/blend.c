/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <GLES3/gl3.h>

#include "compositor/blend.h"
#include "shader/shader.h"
#include "core/debug.h"

static GLuint s_program;
static GLuint s_vbo;
static GLint  s_pos_loc;
static GLint  s_tex_loc[BGEM_LAYER_COUNT];

bgem_result bgem_blend_init(void)
{
    char name[32];

    s_program = bgem_shader_get("blend");
    if (!s_program)
    {
        DEBUG_PRINT("bgem_blend_init: shader 'blend' not found");
        return BGEM_ERROR_IO;
    }

    s_pos_loc = glGetAttribLocation(s_program, "aPos");

    for (int i = 0; i < BGEM_LAYER_COUNT; i++)
    {
        snprintf(name, sizeof(name), "u_layer[%d]", i);
        s_tex_loc[i] = glGetUniformLocation(s_program, name);
    }

    float verts[] = { -1.0f, -1.0f,  3.0f, -1.0f,  -1.0f,  3.0f };
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return BGEM_OK;
}

void bgem_blend_shutdown(void)
{
    glDeleteBuffers(1, &s_vbo);
    s_vbo = 0;
}

void bgem_blend_execute(GLuint layer_tex[BGEM_LAYER_COUNT],
                        GLuint result_fbo, int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, result_fbo);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(s_program);

    for (int i = 0; i < BGEM_LAYER_COUNT; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, layer_tex[i]);
        glUniform1i(s_tex_loc[i], i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glEnableVertexAttribArray(s_pos_loc);
    glVertexAttribPointer(s_pos_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(s_pos_loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
