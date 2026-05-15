/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>
#include "shader/shader_catalog.h"

const bgem_shader_descriptor bgem_shader_catalog[] = {
    { "blit",       "assets/shaders/blit/vertex.glsl",       "assets/shaders/blit/fragment.glsl"       },
    { "testshader", "assets/shaders/testshader/vertex.glsl", "assets/shaders/testshader/fragment.glsl" },
    { NULL, NULL, NULL } /* End of list */
};

const int bgem_shader_catalogCount = (int)(sizeof(bgem_shader_catalog) / sizeof(bgem_shader_catalog[0]));
