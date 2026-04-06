/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include <SDL3/SDL.h>
#include <GLES2/gl2.h>

#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "core/debug.h"

static GLuint program;
static GLuint vbo;
static GLint timeLocation;
static GLint posLocation;

void bgem_renderer_init(void)
{
    const char *basePath;

    basePath = SDL_GetBasePath();
    if (!basePath) {
        SDL_Log("SDL_GetBasePath failed: %s", SDL_GetError());
    }

    char vertexPath[1024];
    snprintf(vertexPath, sizeof(vertexPath), "%sassets/shaders/testshader/vertex.glsl", basePath);
    DEBUG_PRINT("Opening vertex shader at: %s\n", vertexPath);

    char fragmentPath[1024];
    snprintf(fragmentPath, sizeof(fragmentPath), "%sassets/shaders/testshader/fragment.glsl", basePath);
    DEBUG_PRINT("Opening fragment shader at: %s\n", fragmentPath);
    program = bgem_shader_createProgram(
        vertexPath,
        fragmentPath
    );

    posLocation = glGetAttribLocation(program, "aPos");
    timeLocation = glGetUniformLocation(program, "uTime");

    float vertices[] =
    {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void bgem_renderer_render(float time)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glUniform1f(timeLocation, time);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
