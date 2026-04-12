/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "window/window.h"

void bgem_renderer_init(void);
void bgem_renderer_render(float time);
void bgem_renderer_present(bgem_platform_windowContext *platformContext);

#endif
