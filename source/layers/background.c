/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <GLES3/gl3.h>

#include "layers/background.h"
#include "shader/shader.h"
#include "core/debug.h"

static GLuint program;
static GLuint vbo;
static GLint  pos_loc;
static GLint  time_loc;
static float  time_accum;

bgem_result bgem_background_init(void)
{
    program = bgem_shader_get("background");
    if (!program) /* TODO: Maybe redundant? If a call to load all the shaders is done, maybe the checks should be done there. */
    {
        DEBUG_PRINT("bgem_background_init: shader 'background' not found");
        return BGEM_ERROR_IO;
    }

    pos_loc    = glGetAttribLocation(program,  "aPos");
    time_loc   = glGetUniformLocation(program, "uTime");
    time_accum = 0.0f;

    float verts[] = { -1.0f, -1.0f,  3.0f, -1.0f,  -1.0f,  3.0f };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return BGEM_OK;
}

void bgem_background_destroy(void)
{
    glDeleteBuffers(1, &vbo);
    vbo = 0;
}

void bgem_background_update(float dt)
{
    time_accum += dt;
}

void bgem_background_render(void)
{
    /* The FBO is already bound and cleared by the compositor. */
    glUseProgram(program);
    glUniform1f(time_loc, time_accum);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(pos_loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
