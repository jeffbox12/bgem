/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef APP_LOOP_H
#define APP_LOOP_H

#include "window/window.h"
#include "system/config.h"

/**
 * @brief
 * Makes everything move.
 *
 * @param[in] wh
 * The window to control in the loop. Sends commands to the window and
 * the context inside it.
 *
 * @param[in] cfg
 * The struct with the current program configuration.
 *
 * @return
 * BGEM_OK when the loop is ended with no problems. BGEM_ERROR_GPU if a
 * failure occurred within the loop.
 */
bgem_result bgem_app_loop(bgem_window_handle *wh, bgem_config *cfg);

#endif /* APP_LOOP_H */
