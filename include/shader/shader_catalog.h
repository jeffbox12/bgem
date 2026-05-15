/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef SHADER_CATALOG_H
#define SHADER_CATALOG_H

/**
 * @brief
 * Maximum number of shader entries.
 *
 * Used in shader.c in bgem_shader_compiledPrograms[] to
 * create an array of compile shaders to store.
 *
 * Remember to increase this number as more shaders are
 * introduced.
 *
 * TODO: This could be done automatically as more entries
 * are added and the constant might by unnecesary. Review
 * this in the next refactor.
 */
#define BGEM_SHADER_MAX 16

/**
 * @brief
 * Describes a single shader, associating a name and their
 * paths.
 *
 * Shaders are located in "assets/shaders/" and the paths are
 * converted to full paths as it gets called by bgem_shader_loadAll()
 *
 * This is used to build up a catalog of shaders. See
 * shader_catalog.c to see the list of shaders
 */
typedef struct {
    const char *name; /**< Simple shader name, usually matching the file name */
    const char *vert; /**< Relative path to the vertex shader */
    const char *frag; /**< Relative path to the fragment shader */
} bgem_shader_descriptor;

/**
 * @brief
 * Contains the list of shaders and their associated paths.
 *
 * The last item in the list must always be NULL, which can be
 * detected as it is being parsed through.
 */
extern const bgem_shader_descriptor bgem_shader_catalog[];

/**
 * @brief
 * The number of items in the shader catalog list.
 *
 * Always stays in sync with bgem_shader_catalog[] as
 * more items are added.
 */
extern const int bgem_shader_catalogCount;

#endif /* SHADER_CATALOG_H */
