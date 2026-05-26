/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <SDL3/SDL.h>

/* Basic console debug line printing */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[BGEM] %s:%d: " fmt "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

/**
 * @brief
 * List of error codes returned by bgem's functions for error handling
 */
typedef enum {
    BGEM_OK = 0, /**< Executed successfully */
    BGEM_ERROR_GPU, /**< Graphics or rendering error */
    BGEM_ERROR_PLATFORM, /**< Operating system function error */
    BGEM_ERROR_OOM, /**< Out of memory error */
    BGEM_ERROR_IO, /**< File operation error */
    BGEM_ERROR_NOT_FOUND /**< An asset or file is not available */
} bgem_result;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
/**
 * @brief
 * Initialize the Imgui debug UI
 *
 * @param[in] window
 * The window created by SDL. Must be the main Bluegem window
 *
 */
void bgem_debug_init(SDL_Window *window);

/**
 * @brief
 * Destroy the Imgui context
 *
 * Must be called after bgem_debug_init()
 */
void bgem_debug_shutdown(void);

/**
 * @brief
 * On/Off switch to show the UI or not
 */
void bgem_debug_toggle(void);

/**
 * @brief
 * Generate one frame of the ImGui interface
 *
 * It has to be called in a per tick/frame loop.
 *
 * TODO: The user interface is created inside here,
 *       probably has to be moved somewhere else.
 */
void bgem_debug_newFrame(void);

/**
 * @brief
 * Render the debug UI
 */
void bgem_debug_render(void);

/**
 * @brief
 * Returns the status set by bgem_debug_toggle()
 *
 * @return
 * TRUE if enabled. FALSE if disabled.
 */
int  bgem_debug_isActive(void);

/**
 * @brief
 * Forward an SDL event to the ImGui backend.
 *
 * Must be called for every SDL event polled, regardless of the debug
 * overlay's active state. ImGui needs the raw event stream to track
 * mouse, keyboard and text input correctly.
 *
 * @param[in] event
 * The SDL event to forward.
 */
void bgem_debug_processEvent(SDL_Event *event);

#define BGEM_DEBUG_INIT(window)          bgem_debug_init(window)
#define BGEM_DEBUG_SHUTDOWN()            bgem_debug_shutdown()
#define BGEM_DEBUG_TOGGLE()              bgem_debug_toggle()
#define BGEM_DEBUG_NEWFRAME()            bgem_debug_newFrame()
#define BGEM_DEBUG_RENDER()              bgem_debug_render()
#define BGEM_DEBUG_ISACTIVE()            bgem_debug_isActive()
#define BGEM_DEBUG_PROCESSEVENT(event)   bgem_debug_processEvent(event)

#else
#define BGEM_DEBUG_INIT(window)          ((void)0)
#define BGEM_DEBUG_SHUTDOWN()            ((void)0)
#define BGEM_DEBUG_TOGGLE()              ((void)0)
#define BGEM_DEBUG_NEWFRAME()            ((void)0)
#define BGEM_DEBUG_RENDER()              ((void)0)
#define BGEM_DEBUG_ISACTIVE()            ((void)0)
#define BGEM_DEBUG_PROCESSEVENT(event)   ((void)0)

#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
