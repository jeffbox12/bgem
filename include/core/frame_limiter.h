/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef FRAME_LIMITER_H
#define FRAME_LIMITER_H

#include <SDL3/SDL.h>

typedef struct {
    Uint32  target_fps;
    double  frame_budget;   /* seconds per frame (1.0 / target_fps) */
    double  app_start;      /* timestamp at limiter init             */
    double  frame_start;    /* timestamp at last bgem_frame_limiter_begin() */
    float   dt_ms;          /* actual duration of the previous frame */
    double  elapsed;        /* seconds since app_start               */
} bgem_frame_limiter;

/**
 * @brief
 * Initialize the frame limiter.
 *
 * @param[out] fl
 * Fill in the struct members.
 *
 * @param[in] target_fps
 * The framerate number to not go above.
 */
void bgem_frame_limiterInit(bgem_frame_limiter *fl, Uint32 target_fps);

/**
 * @brief
 * The beginning of the frame.
 *
 * Placed at the start of the rendering pipepline. It will create a reference point
 * to compare to when bgem_frame_limiterEnd() is called.
 *
 * A `BGEM_MAX_DT_SECONDS` of 100ms is used to avoid comparing big time differences
 * bugs after suspend or debug breaks.
 *
 * @param[in, out] fl
 * The struct that contains the current frame time information.
 */
void bgem_frame_limiterBegin(bgem_frame_limiter *fl);

/**
 * @brief
 * The end of the frame.
 *
 * Placed at the end of the rendering pipeline. It will compare with the information
 * set from bgem_frame_limiterBegin(), and sleep until it reaches the frame time.
 *
 * @param[in] fl
 * The struct that contains the frame time to compare to.
 */
void bgem_frame_limiterEnd(bgem_frame_limiter *fl);

#endif /* FRAME_LIMITER_H */
