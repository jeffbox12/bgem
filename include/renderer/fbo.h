/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef FBO_H
#define FBO_H

#include <GLES3/gl3.h>

#include "core/debug.h"

/**
 * @brief
 * Create an FBO
 * 
 * @param[in] w
 * Witdth of the FBO in pixels
 * 
 * @param[in] h
 * Height of the FBO in pixels
 * 
 * @param[out] out_fbo
 * The FBO to store to
 * 
 * @param[out] out_tex
 * The FBO texture to store to
 * 
 * @return
 * BGEM_OK if success, BGEM_ERROR_GPU if could not create
 * a FBO correctly.
 */
bgem_result bgem_renderer_fboCreate(int w, int h, GLuint *out_fbo, GLuint *out_tex);

#endif /* FBO_H */
