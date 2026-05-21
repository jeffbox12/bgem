/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shader/shader.h"
#include "shader/shader_catalog.h"
#include "core/path.h"
#include "core/debug.h"

static GLuint bgem_shader_compiledPrograms[BGEM_SHADER_MAX];

static char* read_file(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        DEBUG_PRINT("fopen failed: %s (errno: %s)", path, strerror(errno));
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = 0;

    fclose(file);
    return buffer;
}

static GLuint compile_shader(GLenum type, const char* source)
{
    if(!source) { DEBUG_PRINT("File does not exist"); return EXIT_FAILURE; }
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        DEBUG_PRINT("Shader compile error:\n%s\n", log);
    }

    return shader;
}

GLuint bgem_shader_createProgram(const char* vertexPath, const char* fragmentPath)
{
    char* vertexSource = read_file(vertexPath);
    char* fragmentSource = read_file(fragmentPath);

    if (!vertexSource || !fragmentSource) {
        DEBUG_PRINT("Failed to read shader source path!");
        free(vertexSource);
        free(fragmentSource);
        return EXIT_FAILURE;
    }

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        DEBUG_PRINT("Program link error:\n%s\n", log);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(vertexSource);
    free(fragmentSource);

    return program;
}

void bgem_shader_loadAll(void)
{
    for (int i = 0; bgem_shader_catalog[i].name != NULL; i++)
    {
        char vertPath[BGEM_PATH_MAX];
        char fragPath[BGEM_PATH_MAX];

        bgem_path_relativeToFull(vertPath, sizeof(vertPath), bgem_shader_catalog[i].vert);
        bgem_path_relativeToFull(fragPath, sizeof(fragPath), bgem_shader_catalog[i].frag);

        bgem_shader_compiledPrograms[i] = bgem_shader_createProgram(vertPath, fragPath);
    }
}

GLuint bgem_shader_get(const char *name)
{
    for (int i = 0; bgem_shader_catalog[i].name != NULL; i++)
    {
        if (SDL_strcmp(bgem_shader_catalog[i].name, name) == 0)
            return bgem_shader_compiledPrograms[i];
    }
    return 0; /* Not found */
}

void bgem_shader_destroyAll(void)
{
    for (int i = 0; bgem_shader_catalog[i].name != NULL; i++)
    {
        glDeleteProgram(bgem_shader_compiledPrograms[i]);
        bgem_shader_compiledPrograms[i] = 0;
    }
}
