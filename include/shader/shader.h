/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef SHADER_H
#define SHADER_H

#include <GLES2/gl2.h>

#include "core/debug.h"

GLuint bgem_shader_createProgram(const char* vertexPath, const char* fragmentPath);

/**
 * @brief
 * Reads the shader list form the catalog, compiles them all
 * and stores the GLuint result in a list.
 */
bgem_result bgem_shader_loadAll(void);

/**
 * @brief
 * Fetch a shader from the catalog.
 *
 * @param name
 * The shader name to fetch.
 *
 * @retval 0
 * (GL_NONE) Returns zero if no shader is found.
 *
 * @retval non-zero
 * Returns a valid GLuint value of a shader.
 */
GLuint bgem_shader_get(const char *name);

/**
 * @brief
 * Clears all the shaders
 */
void bgem_shader_destroyAll(void);

#endif /* SHADER_H */
