/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef INPUT_H
#define INPUT_H

#include "core/debug.h"
#include "window/window.h"

/**
 * @brief
 * System-level logical action set.
 *
 * Physical input sources (keyboard keys, gamepad buttons, analog sticks,
 * mouse movement) are translated into these actions by the input module.
 * All platform and application code should query actions exclusively,
 * never raw physical input, so that bindings remain changeable without
 * touching higher-level logic.
 *
 * Actions are source-agnostic, it does not matter whether a direction
 * came from the keyboard, a D-pad, an analog stick, or a mouse delta.
 * Multiple physical inputs can map to the same action simultaneously.
 */
typedef enum {
    /* Navigation */
    BGEM_ACTION_NAVIGATE_UP = 0,
    BGEM_ACTION_NAVIGATE_DOWN,
    BGEM_ACTION_NAVIGATE_LEFT,
    BGEM_ACTION_NAVIGATE_RIGHT,

    /* Interaction */
    BGEM_ACTION_CONFIRM, /**< Accept / select the focused element */
    BGEM_ACTION_BACK,    /**< Cancel / return to previous context */
    BGEM_ACTION_MENU,    /**< Open context menu / start */

    /* System */
    BGEM_ACTION_FULLSCREEN_TOGGLE,
    BGEM_ACTION_DEBUG_TOGGLE, /**< Toggle ImGui debug overlay (debug builds only) */

    /* Do not use as an action index. Always keep this at the bottom. */
    BGEM_ACTION_COUNT
} bgem_action;

/**
 * @brief
 * Initialize the input subsystem.
 *
 * Zeros all internal state and opens any gamepads that are already
 * connected. Must be called after SDL has been initialized with gamepad
 * support, and before the main loop begins.
 *
 * @return BGEM_OK on success, BGEM_ERROR_PLATFORM on failure.
 */
bgem_result bgem_input_init(void);

/**
 * @brief
 * Shut down the input subsystem.
 *
 * Closes all open gamepad handles. Must be called before SDL_Quit.
 */
void bgem_input_shutdown(void);

/**
 * @brief
 * Process all pending SDL events and rebuild action state.
 *
 * Must be called exactly once per frame, before any scene or application
 * code queries the input state. Drains the SDL event queue, translates
 * physical input into logical actions, and gamepad hotplug events internally.
 *
 * Physical input state is always sampled in full regardless of the
 * capture flag, so that the state is accurate the moment capture is
 * released.
 *
 * @param[in] wh
 * The active window handle, used to dispatch resize events. This should be
 * removed as it is not input related.
 *
 * TODO: Replace the window handle parameter with a registered callback
 * so the input module has no direct dependency on the window module.
 */
void bgem_input_update(bgem_window_handle *wh);

/**
 * @brief
 * Returns 1 if the action became active this frame.
 *
 * True for exactly one frame — the frame on which physical input was
 * first detected. Use this for single-fire events such as confirming a
 * selection or triggering a toggle.
 *
 * When input is captured, returns 0 for all actions except
 * BGEM_ACTION_DEBUG_TOGGLE, which always fires.
 */
int bgem_input_actionJustPressed(bgem_action action);

/**
 * @brief
 * Returns 1 if the action is currently active.
 *
 * True for every frame the physical input remains held. Application code
 * is responsible for implementing hold-to-repeat behaviour on top of
 * this signal when needed; the input module does not produce repeat
 * events, by design.
 *
 * When input is captured, returns 0 for all actions except
 * BGEM_ACTION_DEBUG_TOGGLE, which always fires.
 */
int bgem_input_actionHeld(bgem_action action);

/**
 * @brief
 * Returns 1 if the action became inactive this frame.
 *
 * True for exactly one frame — the frame on which physical input was
 * released.
 *
 * When input is captured, returns 0 for all actions except
 * BGEM_ACTION_DEBUG_TOGGLE, which always fires.
 */
int bgem_input_actionJustReleased(bgem_action action);

/**
 * @brief
 * Returns 1 if a quit event has been received.
 *
 * Reflects SDL_EVENT_QUIT, no matter if the debug overlay is opened
 * or closed.
 *
 * TODO: This is not input related. This should be moved elsewhere.
 */
int bgem_input_quitRequested(void);

/**
 * @brief
 * Set whether input is captured by a debug overlay.
 *
 * When captured, all action queries return 0 except BGEM_ACTION_DEBUG_TOGGLE,
 * which remains active so the overlay can be dismissed. Physical state
 * continues to be sampled normally.
 *
 * @param[in] captured
 * 1 to capture input, 0 to release it.
 */
void bgem_input_setDebugUICapture(int captured);

/**
 * @brief
 * Returns the current capture state.
 *
 * @return
 * 1 if input is currently captured, 0 otherwise.
 */
int bgem_input_isCaptured(void);

#endif /* INPUT_H */
