/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>
#include "shader/shader_catalog.h"

const bgem_shader_descriptor bgem_shader_catalog[] = {
    { "blit",       "assets/shaders/blit/vertex.glsl",       "assets/shaders/blit/fragment.glsl"       },
    { "testshader", "assets/shaders/testshader/vertex.glsl", "assets/shaders/testshader/fragment.glsl" },
    { "background", "assets/shaders/background/vertex.glsl", "assets/shaders/background/fragment.glsl" },
    { "blend",      "assets/shaders/blend/vertex.glsl",      "assets/shaders/blend/fragment.glsl"      },
    { "text",       "assets/shaders/text/vertex.glsl",       "assets/shaders/text/fragment.glsl"       },
    { NULL, NULL, NULL } /* End of list */
};

const int bgem_shader_catalogCount = (int)(sizeof(bgem_shader_catalog) / sizeof(bgem_shader_catalog[0]));
