/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include "core/frame_limiter.h"
#include "core/timer.h"

/* Clamp dt to 100ms — prevents explosion after suspend, breakpoints, etc. */
#define BGEM_MAX_DT_SECONDS 0.1

void bgem_frame_limiterInit(bgem_frame_limiter *fl, Uint32 target_fps)
{
    fl->target_fps   = target_fps;
    fl->frame_budget = 1.0 / (double)target_fps;
    fl->app_start    = bgem_timer_seconds();
    fl->frame_start  = fl->app_start;
    fl->dt_ms        = 0.0f;
    fl->elapsed      = 0.0;
}

void bgem_frame_limiterBegin(bgem_frame_limiter *fl)
{
    double now    = bgem_timer_seconds();
    double raw_dt = now - fl->frame_start;

    if (raw_dt > BGEM_MAX_DT_SECONDS)
        raw_dt = BGEM_MAX_DT_SECONDS;

    fl->dt_ms       = (float)(raw_dt * 1000.0);
    fl->elapsed     = now - fl->app_start;
    fl->frame_start = now;
}

void bgem_frame_limiterEnd(bgem_frame_limiter *fl)
{
    double elapsed_this_frame = bgem_timer_seconds() - fl->frame_start;
    double remaining          = fl->frame_budget - elapsed_this_frame;

    /* Sleep the bulk of the remaining budget, leaving ~2ms for the spin */
    if (remaining > 0.002)
        SDL_DelayNS((Uint64)((remaining - 0.002) * 1e9));

    /* Spin-wait the final fraction for precision */
    while (bgem_timer_seconds() - fl->frame_start < fl->frame_budget)
        { /* spin */ }
}
