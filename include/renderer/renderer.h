/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "window/window.h"

void bgem_renderer_setWindowSize(int w, int h);
void bgem_renderer_init(void);
void bgem_renderer_render(float time);
void bgem_renderer_present(int window_width, int window_height);
void bgem_renderer_swap(bgem_platform_windowContext *ctx);

#endif
