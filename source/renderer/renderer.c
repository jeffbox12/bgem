/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include <SDL3/SDL.h>
#include <GLES2/gl2.h>

#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "platform/platform_window.h"
#include "core/debug.h"

/* ── Scene state ───────────────────────────────────────────────────────────── */
static GLuint program;
static GLuint vbo;
static GLint  timeLocation;
static GLint  posLocation;

/* ── Offscreen FBO ─────────────────────────────────────────────────────────── */
static GLuint fbo;
static GLuint fbo_texture;

/* ── Blit quad ─────────────────────────────────────────────────────────────── */
static GLuint blit_program;
static GLuint blit_vbo;
static GLint  blit_texLocation;
static GLint  blit_posLocation;

/* ── Resolutions ───────────────────────────────────────────────────────────── */
static int internal_w = 16;
static int internal_h = 9;
static int window_w   = 1280;   /* updated via bgem_renderer_setWindowSize() */
static int window_h   = 720;

/* ── Blit shader sources (inline, no files needed) ─────────────────────────── */
static const char *blit_vert_src =
    "#version 300 es\n"
    "in vec2 aPos;\n"
    "out vec2 vUV;\n"
    "void main() {\n"
    "    vUV = aPos * 0.5 + 0.5;\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const char *blit_frag_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 vUV;\n"
    "uniform sampler2D uTex;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = texture(uTex, vUV);\n"
    "}\n";

/* ── Helpers ────────────────────────────────────────────────────────────────── */
static GLuint compile_inline(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        DEBUG_PRINT("Blit shader compile error:\n%s\n", log);
    }
    return shader;
}

static GLuint link_inline(const char *vert, const char *frag)
{
    GLuint vs  = compile_inline(GL_VERTEX_SHADER,   vert);
    GLuint fs  = compile_inline(GL_FRAGMENT_SHADER, frag);
    GLuint prg = glCreateProgram();
    glAttachShader(prg, vs);
    glAttachShader(prg, fs);
    glLinkProgram(prg);

    GLint ok;
    glGetProgramiv(prg, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prg, sizeof(log), NULL, log);
        DEBUG_PRINT("Blit program link error:\n%s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prg;
}

static void create_fbo(void)
{
    /* Color texture at internal resolution */
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 internal_w, internal_h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Change back to GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* FBO */
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fbo_texture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        DEBUG_PRINT("FBO incomplete: 0x%x\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* ─────────────────────────────────────────────────────────────────────────── */

void bgem_renderer_setWindowSize(int w, int h)
{
    window_w = w;
    window_h = h;
}

void bgem_renderer_init(void)
{
    /* ── Scene shader ───────────────────────────────────────────────────── */
    const char *basePath = SDL_GetBasePath();
    if (!basePath)
        SDL_Log("SDL_GetBasePath failed: %s", SDL_GetError());

    char vertexPath[1024];
    snprintf(vertexPath, sizeof(vertexPath),
             "%sassets/shaders/testshader/vertex.glsl", basePath);
    DEBUG_PRINT("Opening vertex shader at: %s\n", vertexPath);

    char fragmentPath[1024];
    snprintf(fragmentPath, sizeof(fragmentPath),
             "%sassets/shaders/testshader/fragment.glsl", basePath);
    DEBUG_PRINT("Opening fragment shader at: %s\n", fragmentPath);

    program     = bgem_shader_createProgram(vertexPath, fragmentPath);
    posLocation = glGetAttribLocation(program, "aPos");
    timeLocation = glGetUniformLocation(program, "uTime");

    float vertices[] = { -1.0f, -1.0f,  3.0f, -1.0f,  -1.0f,  3.0f };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* ── Offscreen FBO ─────────────────────────────────────────────────── */
    create_fbo();

    /* ── Blit quad ─────────────────────────────────────────────────────── */
    blit_program    = link_inline(blit_vert_src, blit_frag_src);
    blit_posLocation = glGetAttribLocation(blit_program, "aPos");
    blit_texLocation = glGetUniformLocation(blit_program, "uTex");

    /* Reuse the same fullscreen triangle geometry */
    glGenBuffers(1, &blit_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void bgem_renderer_render(float time)
{
    /* ── Render scene into FBO at internal resolution ───────────────────── */
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, internal_w, internal_h);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glUniform1f(timeLocation, time);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bgem_renderer_present(bgem_platform_windowContext *ctx, int window_width, int window_height)
{
    /* ── Compute a letterboxed/pillarboxed viewport ──────────────────────── */
    float scale_x = (float)window_width / (float)internal_w;
    float scale_y = (float)window_height / (float)internal_h;
    float scale   = scale_x < scale_y ? scale_x : scale_y;   /* fit, no crop */

    int draw_w  = (int)(internal_w * scale);
    int draw_h  = (int)(internal_h * scale);
    int offset_x = (window_width - draw_w) / 2;
    int offset_y = (window_height - draw_h) / 2;

    /* Clear the full window (shows letterbox bars in clear color) */
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Blit FBO texture into the fitted rect */
    glViewport(offset_x, offset_y, draw_w, draw_h);
    glUseProgram(blit_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glUniform1i(blit_texLocation, 0);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    glEnableVertexAttribArray(blit_posLocation);
    glVertexAttribPointer(blit_posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    bgem_platform_swapBuffers(ctx);
}
