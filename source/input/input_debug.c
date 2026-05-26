/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdbool.h>
#include "input/input.h"
#include "input/input_debug.h"
#include "core/debug.h"
#include "core/debug_defaults.h"

/**
 * @brief
 * Action names parallel to the bgem_action enum, used for debug logging
 */
static const char *s_action_names[BGEM_ACTION_COUNT] = {
    "NAVIGATE_UP",
    "NAVIGATE_DOWN",
    "NAVIGATE_LEFT",
    "NAVIGATE_RIGHT",
    "CONFIRM",
    "BACK",
    "MENU",
    "FULLSCREEN_TOGGLE",
    "DEBUG_TOGGLE",
};

void bgem_input_debug(void)
{
    /*
     * Debug toggle is exempt from the capture check inside the input
     * module, so this fires correctly in both directions: opening the
     * overlay when input is free, and closing it when input is captured.
     */
#ifdef DEBUG
    if (bgem_input_actionJustPressed(BGEM_ACTION_DEBUG_TOGGLE)) {
        BGEM_DEBUG_TOGGLE();
        bgem_input_setDebugUICapture(BGEM_DEBUG_ISACTIVE());
    }
#endif

    /* Log every action that fires this frame so we can verify bindings */
    if (inputLog == true)
    {
        for (int a = 0; a < BGEM_ACTION_COUNT; a++) {
            if (bgem_input_actionJustPressed((bgem_action)a)) {
                DEBUG_PRINT("[input] just_pressed  : %s", s_action_names[a]);
            }
            if (bgem_input_actionJustReleased((bgem_action)a)) {
                DEBUG_PRINT("[input] just_released : %s", s_action_names[a]);
            }
        }
    }

}
