/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <wayland-egl.h>

#include "platform/platform_window.h"
#include "core/debug.h"

/* Internal context */
struct bgem_platform_windowContext
{
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    struct wl_egl_window *wl_win;
};

bgem_platform_windowContext* bgem_platform_createContext(SDL_Window *window)
{
    bgem_platform_windowContext* ctx;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    EGLint numConfigs;
    EGLNativeWindowType nativeWindow;
    Uint64 x11Window;
    SDL_PropertiesID props;
    struct wl_display* wl_dpy;
    struct wl_surface* wl_surf;
    struct wl_egl_window* wl_win;

    props = SDL_GetWindowProperties(window);

    const char* driver = SDL_GetCurrentVideoDriver();

    if (driver)
    {
        if (strcmp(driver, "wayland") == 0)
        {
            DEBUG_PRINT("Environment: Native Wayland\n");
            wl_dpy = SDL_GetPointerProperty(props,
                                            SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
                                            NULL);
            wl_surf = SDL_GetPointerProperty(props,
                                             SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER,
                                             NULL);
            if (!wl_dpy || !wl_surf)
            {
                DEBUG_PRINT("[SDL] Could not get Wayland handles. Is the video driver actually Wayland? (current: %s)\n",
                            driver);
                return NULL;
            }

            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            wl_win = wl_egl_window_create(wl_surf, w, h);
            if (!wl_win)
            {
                DEBUG_PRINT("[Wayland] wl_egl_window_create failed\n");
                return NULL;
            }

            nativeWindow = (EGLNativeWindowType)wl_win;
            display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_KHR, wl_dpy, NULL);
            if (display == EGL_NO_DISPLAY) return NULL;
        }
        else if (strcmp(driver, "x11") == 0)
        {
            DEBUG_PRINT("Environment: X11 (or XWayland)\n");
            wl_win = NULL;
            x11Window = SDL_GetNumberProperty(props,
                                              SDL_PROP_WINDOW_X11_WINDOW_NUMBER,
                                              0);

            if (!x11Window) return NULL;

            nativeWindow = (EGLNativeWindowType)x11Window;
            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (display == EGL_NO_DISPLAY) return NULL;
            }
            else
            {
                DEBUG_PRINT("Environment: %s\n", driver);
                return NULL;
            }
    }

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
                         &config,
                         1,
                         &numConfigs))
        return NULL;

    const EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    context = eglCreateContext(display,
                               config,
                               EGL_NO_CONTEXT,
                               contextAttribs);

    if (context == EGL_NO_CONTEXT) return NULL;

    surface = eglCreateWindowSurface(display,
                                     config,
                                     nativeWindow,
                                     NULL);

    if (surface == EGL_NO_SURFACE) return NULL;

    if (!eglMakeCurrent(display, surface, surface, context)) return NULL;

    ctx = (bgem_platform_windowContext*)malloc(sizeof(bgem_platform_windowContext));

    ctx->display = display;
    ctx->surface = surface;
    ctx->context = context;
    ctx->wl_win = wl_win;

    return ctx;
}

void bgem_platform_swapBuffers(bgem_platform_windowContext *ctx)
{
    eglSwapBuffers(ctx->display, ctx->surface);
}

void bgem_platform_waylandResizeSurface(bgem_platform_windowContext *ctx, int *w, int *h)
{
    if (ctx->wl_win) wl_egl_window_resize(ctx->wl_win, w, h, 0, 0);
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
    if (ctx->wl_win) wl_egl_window_destroy(ctx->wl_win); // If NULL means X11
    eglDestroyContext(ctx->display, ctx->context);
    eglTerminate(ctx->display);

    free(ctx);
}
