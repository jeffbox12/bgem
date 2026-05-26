/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <string.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#include "input/input.h"
#include "input/input_debug.h"
#include "window/window.h"

#define INPUT_MAX_GAMEPADS 8

/**
 * Analog stick axis thresholds using hysteresis to prevent the
 * directonal action from flickering.
 */
#define AXIS_ACTIVATE_THRESHOLD 10000
#define AXIS_RELEASE_THRESHOLD   8000

/**
 * Scroll wheel accumulation threshold. One navigation step fires when
 * the accumulated delta reaches this value. 1.0 corresponds to one
 * standard wheel notch; devices that send fractional deltas (i.e. trackpads)
 * accumulate across events before firing.
 */
#define SCROLL_THRESHOLD 1.0f

typedef struct {
    bgem_action  action;
    SDL_Scancode scancode;
} bgem_keyboard_binding;

typedef struct {
    bgem_action action;
    SDL_MouseButtonFlags button;
} bgem_mouse_binding;

typedef struct {
    bgem_action       action;
    SDL_GamepadButton button;
} bgem_gamepad_button_binding;
typedef struct {
    bgem_action     action;
    SDL_GamepadAxis axis;
    int             positive; /* 1 = positive direction, 0 = negative */
} bgem_gamepad_axis_binding;

/* ── Default bindings ───────────────────────────────────────────────────── */

static const bgem_keyboard_binding s_keyboard_bindings[] = {
//    Action                         Scancode
    { BGEM_ACTION_NAVIGATE_UP,       SDL_SCANCODE_UP       },
    { BGEM_ACTION_NAVIGATE_UP,       SDL_SCANCODE_W        },
    { BGEM_ACTION_NAVIGATE_DOWN,     SDL_SCANCODE_DOWN     },
    { BGEM_ACTION_NAVIGATE_DOWN,     SDL_SCANCODE_S        },
    { BGEM_ACTION_NAVIGATE_LEFT,     SDL_SCANCODE_LEFT     },
    { BGEM_ACTION_NAVIGATE_LEFT,     SDL_SCANCODE_A        },
    { BGEM_ACTION_NAVIGATE_RIGHT,    SDL_SCANCODE_RIGHT    },
    { BGEM_ACTION_NAVIGATE_RIGHT,    SDL_SCANCODE_D        },
    { BGEM_ACTION_CONFIRM,           SDL_SCANCODE_RETURN   },
    { BGEM_ACTION_CONFIRM,           SDL_SCANCODE_SPACE    },
    { BGEM_ACTION_CONFIRM,           SDL_SCANCODE_KP_ENTER },
    { BGEM_ACTION_BACK,              SDL_SCANCODE_ESCAPE   },
    { BGEM_ACTION_BACK,              SDL_SCANCODE_BACKSPACE},
    { BGEM_ACTION_FULLSCREEN_TOGGLE, SDL_SCANCODE_F11      },
    { BGEM_ACTION_DEBUG_TOGGLE,      SDL_SCANCODE_B        },
};

static const bgem_mouse_binding s_mouse_bindings[] = {
//    Action               Button
    { BGEM_ACTION_CONFIRM, SDL_BUTTON_LEFT  },
    { BGEM_ACTION_MENU,    SDL_BUTTON_RIGHT },
};

static const bgem_gamepad_button_binding s_gamepad_button_bindings[] = {
//    Action                      Button
    { BGEM_ACTION_NAVIGATE_UP,    SDL_GAMEPAD_BUTTON_DPAD_UP    },
    { BGEM_ACTION_NAVIGATE_DOWN,  SDL_GAMEPAD_BUTTON_DPAD_DOWN  },
    { BGEM_ACTION_NAVIGATE_LEFT,  SDL_GAMEPAD_BUTTON_DPAD_LEFT  },
    { BGEM_ACTION_NAVIGATE_RIGHT, SDL_GAMEPAD_BUTTON_DPAD_RIGHT },
    { BGEM_ACTION_CONFIRM,        SDL_GAMEPAD_BUTTON_SOUTH      },
    { BGEM_ACTION_BACK,           SDL_GAMEPAD_BUTTON_EAST       },
    { BGEM_ACTION_MENU,           SDL_GAMEPAD_BUTTON_START      },
    { BGEM_ACTION_MENU,           SDL_GAMEPAD_BUTTON_BACK       },
};

static const bgem_gamepad_axis_binding s_gamepad_axis_bindings[] = {
//    Action                      Button                  Positive
    { BGEM_ACTION_NAVIGATE_RIGHT, SDL_GAMEPAD_AXIS_LEFTX, true  },
    { BGEM_ACTION_NAVIGATE_LEFT,  SDL_GAMEPAD_AXIS_LEFTX, false },
    { BGEM_ACTION_NAVIGATE_DOWN,  SDL_GAMEPAD_AXIS_LEFTY, true  },
    { BGEM_ACTION_NAVIGATE_UP,    SDL_GAMEPAD_AXIS_LEFTY, false },
};

#define KEYBOARD_BINDING_COUNT       (int)(sizeof(s_keyboard_bindings) / sizeof(s_keyboard_bindings[0]))
#define MOUSE_BINDING_COUNT          (int)(sizeof(s_mouse_bindings) / sizeof(s_mouse_bindings[0]))
#define GAMEPAD_BUTTON_BINDING_COUNT (int)(sizeof(s_gamepad_button_bindings) / sizeof(s_gamepad_button_bindings[0]))
#define GAMEPAD_AXIS_BINDING_COUNT   (int)(sizeof(s_gamepad_axis_bindings) / sizeof(s_gamepad_axis_bindings[0]))

/* ── Module state ───────────────────────────────────────────────────────── */

/* Action state for the current and previous frame */
static int s_current[BGEM_ACTION_COUNT];
static int s_previous[BGEM_ACTION_COUNT];

/*
 * Per-gamepad-slot, per-axis-binding hysteresis state.
 * Tracks whether each axis direction was already "crossed" last frame,
 * so the digital action does not toggle within a single frame when the
 * stick rests near the threshold.
 */
static int s_axis_active[INPUT_MAX_GAMEPADS][GAMEPAD_AXIS_BINDING_COUNT];

/* Open gamepad handles, indexed by slot */
static SDL_Gamepad *s_gamepads[INPUT_MAX_GAMEPADS];

/* Set to 1 on SDL_EVENT_QUIT; never cleared during the session */
static int s_quit_requested;

/*
 * Per-frame scroll wheel accumulator. Deltas are summed within
 * a frame and consumed against SCROLL_THRESHOLD after the
 * event queue is drained.
 */
static float s_scroll_accum_x;
static float s_scroll_accum_y;

/*
 * When 1, all action queries return 0 except BGEM_ACTION_DEBUG_TOGGLE,
 * which remains active so the overlay that set capture can be dismissed.
 */
static int s_captured;

/* ── Internal helpers ───────────────────────────────────────────────────── */

static void input_openGamepad(SDL_JoystickID id)
{
    for (int i = 0; i < INPUT_MAX_GAMEPADS; i++) {
        if (!s_gamepads[i]) {
            s_gamepads[i] = SDL_OpenGamepad(id);
            if (s_gamepads[i]) {
                DEBUG_PRINT("Gamepad connected in slot %d: %s",
                    i, SDL_GetGamepadName(s_gamepads[i]));
            } else {
                DEBUG_PRINT("SDL_OpenGamepad failed: %s", SDL_GetError());
            }
            return;
        }
    }
    DEBUG_PRINT("Gamepad connected but no free slot available (max %d)",
        INPUT_MAX_GAMEPADS);
}

static void input_closeGamepad(SDL_JoystickID id)
{
    for (int i = 0; i < INPUT_MAX_GAMEPADS; i++) {
        if (s_gamepads[i] && SDL_GetGamepadID(s_gamepads[i]) == id) {
            DEBUG_PRINT("Gamepad disconnected from slot %d", i);
            SDL_CloseGamepad(s_gamepads[i]);
            s_gamepads[i] = NULL;
            /* Reset axis status to prevent ghost control when disconnected while in action */
            memset(s_axis_active[i], 0, sizeof(s_axis_active[i]));
            return;
        }
    }
}

static void input_sampleKeyboard(void)
{
    const bool *keys = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < KEYBOARD_BINDING_COUNT; i++) {
        if (keys[s_keyboard_bindings[i].scancode]) {
            s_current[s_keyboard_bindings[i].action] = true;
        }
    }
}

static void input_sampleMouse(void)
{
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(NULL, NULL);
    for (int i = 0; i < MOUSE_BINDING_COUNT; i++) {
        if (buttons & SDL_BUTTON_MASK(s_mouse_bindings[i].button)) {
            s_current[s_mouse_bindings[i].action] = true;
        }
    }
}

static void input_sampleGamepads(void)
{
    for (int g = 0; g < INPUT_MAX_GAMEPADS; g++) {
        if (!s_gamepads[g]) continue;

        /* Digital buttons */
        for (int i = 0; i < GAMEPAD_BUTTON_BINDING_COUNT; i++) {
            if (SDL_GetGamepadButton(s_gamepads[g],
                    s_gamepad_button_bindings[i].button)) {
                s_current[s_gamepad_button_bindings[i].action] = true;
            }
        }

        /* Analog axes, translated to digital actions */
        for (int i = 0; i < GAMEPAD_AXIS_BINDING_COUNT; i++) {
            Sint16 raw = SDL_GetGamepadAxis(s_gamepads[g],
                s_gamepad_axis_bindings[i].axis);

            /*
             * Flip the sign for negative-direction bindings so the
             * threshold comparison is always against a positive value.
             */
            int val = s_gamepad_axis_bindings[i].positive ? (int)raw : -(int)raw;

            if (s_axis_active[g][i]) {
                if (val < AXIS_RELEASE_THRESHOLD)
                    s_axis_active[g][i] = 0;
            } else {
                if (val > AXIS_ACTIVATE_THRESHOLD)
                    s_axis_active[g][i] = 1;
            }

            if (s_axis_active[g][i]) {
                s_current[s_gamepad_axis_bindings[i].action] = true;
            }
        }
    }
}

/* ── Public API ─────────────────────────────────────────────────────────── */

bgem_result bgem_input_init(void)
{
    memset(s_current,     false, sizeof(s_current));
    memset(s_previous,    false, sizeof(s_previous));
    memset(s_gamepads,    0,     sizeof(s_gamepads));
    memset(s_axis_active, 0,     sizeof(s_axis_active));
    s_quit_requested = 0;
    s_captured       = 0;
    s_scroll_accum_x = 0.0f;
    s_scroll_accum_y = 0.0f;

    /* Open any gamepads that are already connected at startup */
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    if (ids) {
        for (int i = 0; i < count; i++) {
            input_openGamepad(ids[i]);
        }
        SDL_free(ids);
    }

    DEBUG_PRINT("Input subsystem initialized (%d gamepad(s) found)", count);
    return BGEM_OK;
}

void bgem_input_shutdown(void)
{
    for (int i = 0; i < INPUT_MAX_GAMEPADS; i++) {
        if (s_gamepads[i]) {
            SDL_CloseGamepad(s_gamepads[i]);
            s_gamepads[i] = NULL;
        }
    }
    DEBUG_PRINT("Input subsystem shut down");
}

void bgem_input_update(bgem_window_handle *wh)
{
    /* Snapshot the previous frame's state, then clear current for rebuilding */
    memcpy(s_previous, s_current, sizeof(s_current));
    memset(s_current,  0,         sizeof(s_current));

    /* Drain the SDL event queue */
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        /*
         * Always forward every event to ImGui only if the debug status is active.
         * ImGui needs the raw event stream to function. Bgem's capture flag
         * is a query-side concern, not an event-filtering concern.
         */
        INPUT_DEBUG_PROCESSEVENT(&event);

        switch (event.type) {
            case SDL_EVENT_QUIT:
                s_quit_requested = 1;
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:   /* TODO: Move this somewhere else! */
                bgem_window_handleResize(wh, &event);
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                input_openGamepad(event.gdevice.which);
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                input_closeGamepad(event.gdevice.which);
                break;

            case SDL_EVENT_MOUSE_WHEEL: {
                float dir = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1.0f : 1.0f;
                s_scroll_accum_x += event.wheel.x * dir;
                s_scroll_accum_y += event.wheel.y * dir;
                break;
            }

            default:
                break;
        }
    }

    /* Consume the scroll accumulator against the threshold. */
    if (s_scroll_accum_y >= SCROLL_THRESHOLD)
    {
        s_current[BGEM_ACTION_NAVIGATE_UP] = 1;
        s_scroll_accum_y -= SCROLL_THRESHOLD;
        if (s_scroll_accum_y > SCROLL_THRESHOLD)
            s_scroll_accum_y = 0.0f; /* discard excess to avoid queued bursts */
    } else if (s_scroll_accum_y <= -SCROLL_THRESHOLD)
    {
        s_current[BGEM_ACTION_NAVIGATE_DOWN] = 1;
        s_scroll_accum_y += SCROLL_THRESHOLD;
        if (s_scroll_accum_y < -SCROLL_THRESHOLD)
            s_scroll_accum_y = 0.0f;
    }

    if (s_scroll_accum_x >= SCROLL_THRESHOLD)
    {
        s_current[BGEM_ACTION_NAVIGATE_RIGHT] = 1;
        s_scroll_accum_x -= SCROLL_THRESHOLD;
        if (s_scroll_accum_x > SCROLL_THRESHOLD)
            s_scroll_accum_x = 0.0f;
    } else if (s_scroll_accum_x <= -SCROLL_THRESHOLD)
    {
        s_current[BGEM_ACTION_NAVIGATE_LEFT] = 1;
        s_scroll_accum_x += SCROLL_THRESHOLD;
        if (s_scroll_accum_x < -SCROLL_THRESHOLD)
            s_scroll_accum_x = 0.0f;
    }

    /**
     * Set the action table state based on the input presses. Updated every
     * time bgem_input_update() is called.
     */
    input_sampleKeyboard();
    input_sampleMouse();
    input_sampleGamepads();
}

int bgem_input_actionJustPressed(bgem_action action)
{
    if (s_captured && action != BGEM_ACTION_DEBUG_TOGGLE) return 0;
    return s_current[action] && !s_previous[action];
}

int bgem_input_actionHeld(bgem_action action)
{
    if (s_captured && action != BGEM_ACTION_DEBUG_TOGGLE) return 0;
    return s_current[action];
}

int bgem_input_actionJustReleased(bgem_action action)
{
    if (s_captured && action != BGEM_ACTION_DEBUG_TOGGLE) return 0;
    return !s_current[action] && s_previous[action];
}

int bgem_input_quitRequested(void) /* TODO: This might not belong here */
{
    return s_quit_requested;
}

void bgem_input_setDebugUICapture(int captured)
{
    s_captured = captured;
    DEBUG_PRINT("Debug overlay input capture %s", captured ? "enabled" : "disabled");
}

int bgem_input_isCaptured(void)
{
    return s_captured;
}
