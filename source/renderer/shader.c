/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdio.h>
#include <stdlib.h>

#include "renderer/shader.h"
#include "core/debug.h"


static char* ReadFile(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = 0;

    fclose(file);
    return buffer;
}

static GLuint CompileShader(GLenum type, const char* source)
{
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
    char* vertexSource = ReadFile(vertexPath);
    char* fragmentSource = ReadFile(fragmentPath);

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

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
