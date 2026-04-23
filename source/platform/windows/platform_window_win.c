/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <windows.h>
#include <SDL3/SDL.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "platform/platform_window.h"

/* Internal context */
struct bgem_platform_windowContext
{
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};

bgem_platform_windowContext* bgem_platform_createContext(SDL_Window *window)
{
    HWND hwnd;
    void* hwndPtr;
    bgem_platform_windowContext* ctx;
    SDL_PropertiesID props;
    EGLDisplay display;
    EGLConfig config;
    EGLint numConfigs;
    EGLContext context;
    EGLSurface surface;

    props = SDL_GetWindowProperties(window);


    hwndPtr = SDL_GetPointerProperty(props,
                                     SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                     NULL);

    if (!hwndPtr) return NULL;

    hwnd = (HWND)hwndPtr;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) return NULL;

    if (!eglInitialize(display, NULL, NULL)) return NULL;

    eglBindAPI(EGL_OPENGL_ES_API);

    const EGLint configAttribs[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      24,
        EGL_STENCIL_SIZE,    8,
        EGL_NONE
    };

    if (!eglChooseConfig(display,
                         configAttribs,
                         &config, 1, &numConfigs))
        return NULL;

    const EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    context = eglCreateContext(display, config,
                               EGL_NO_CONTEXT,
                               contextAttribs);

    if (context == EGL_NO_CONTEXT) return NULL;

    surface = eglCreateWindowSurface(display,
                                     config,
                                     hwnd,
                                     NULL);

    if (surface == EGL_NO_SURFACE) return NULL;

    if (!eglMakeCurrent(display,
                        surface,
                        surface,
                        context))
        return NULL;

    ctx = (bgem_platform_windowContext*)malloc(sizeof(bgem_platform_windowContext));

    ctx->display = display;
    ctx->surface = surface;
    ctx->context = context;

    return ctx;
}

void bgem_platform_swapBuffers(bgem_platform_windowContext *ctx)
{
    eglSwapBuffers(ctx->display, ctx->surface);
}
void bgem_platform_getSurfaceSize(bgem_platform_windowContext *ctx, int *w, int *h)
{
    eglQuerySurface(ctx->display, ctx->surface, EGL_WIDTH,  w);
    eglQuerySurface(ctx->display, ctx->surface, EGL_HEIGHT, h);
}

void bgem_platform_destroyContext(bgem_platform_windowContext *ctx)
{
    if (!ctx) return;

    eglMakeCurrent(ctx->display,
                   EGL_NO_SURFACE,
                   EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);

    eglDestroySurface(ctx->display, ctx->surface);
    eglDestroyContext(ctx->display, ctx->context);
    eglTerminate(ctx->display);

    free(ctx);
}
