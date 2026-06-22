/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef TEXT_H
#define TEXT_H

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Maximum number of concurrently live text objects.
 *
 * Fixed-size, the same shape as bgem_shader_compiledPrograms[] in
 * shader.c. This prototype never creates objects past init time, so a
 * small static cap is enough.
 *
 * Remember to increase this if more slots are needed.
 */
#define BGEM_TEXT_MAX 16

/**
 * @brief
 * Selects which field bgem_text_editParam() mutates.
 */
typedef enum {
    BGEM_TEXT_PARAM_POSX = 0,
    BGEM_TEXT_PARAM_POSY,
    BGEM_TEXT_PARAM_RED,
    BGEM_TEXT_PARAM_GREEN,
    BGEM_TEXT_PARAM_BLUE,
    BGEM_TEXT_PARAM_ALPHA
} bgem_text_param;

/**
 * @brief
 * Initializes the text system: loads the font/atlas (bgem_font_init())
 * and fetches the compiled "text" shader from the catalog.
 *
 * Must be called after bgem_shader_loadAll(), and before any layer's
 * init() that calls bgem_text_addText() - any layer (UI, Overlay,
 * Notification, ...) can own text objects, so this has to be ready
 * before all of them, not just one.
 *
 * @param[in] screen_width
 * Compositor's internal render width, in pixels. Used to convert pixel
 * positions to NDC in the text shader.
 *
 * @param[in] screen_height
 * Compositor's internal render height, in pixels.
 */
bgem_result bgem_text_init(int screen_width, int screen_height);

/** Destroys every live text object and the font/atlas owned by this module. */
void bgem_text_shutdown(void);

/**
 * @brief
 * Creates a text object in slot `id`.
 *
 * Shapes `str` with Harfbuzz and builds its glyph-quad VBO immediately -
 * this is the expensive call. Meant to run once, typically from a
 * layer's init(), not every frame from render().
 *
 * If `id` is already in use, its previous GL resources are freed first
 * and it's rebuilt in place. A DEBUG_PRINT notes this happened, but it's
 * not treated as an error - this is a rough prototype, not a guarded API,
 * and silently overwriting beats forcing every call site to handle a
 * collision error it probably doesn't care about.
 *
 * @param[in] id            0..BGEM_TEXT_MAX-1. Caller-chosen slot.
 * @param[in] posX, posY    Top-left pixel position of the text's line box,
 *                           in the compositor's internal canvas space
 *                           (origin top-left, Y increasing downward).
 * @param[in] r, g, b, a    Color, each in [0,1].
 * @param[in] str           Null-terminated UTF-8 string.
 *
 * @return BGEM_OK on success, BGEM_ERROR_IO if `id` is out of range,
 * BGEM_ERROR_OOM if a host allocation fails while building geometry.
 */
bgem_result bgem_text_addText(int id, float posX, float posY,
                               float r, float g, float b, float a,
                               const char *str);

/**
 * @brief
 * Replaces the string content of an existing text object, re-shaping it
 * and rebuilding its VBO. Position and color are untouched.
 *
 * Use this - not bgem_text_removeText() + bgem_text_addText() - for
 * anything that changes its text at runtime (e.g. a counter). This only
 * rebuilds the geometry, which is the only part that actually changed.
 *
 * @return BGEM_OK on success, BGEM_ERROR_IO if `id` is out of range,
 * BGEM_ERROR_NOT_FOUND if `id` has no object yet.
 */
bgem_result bgem_text_setString(int id, const char *str);

/**
 * @brief
 * Cheap, per-frame-safe edit of a single position or color field.
 *
 * Touches only a CPU-side value applied as a uniform at render time - no
 * GL buffer is rebuilt, so this is fine to call every frame (e.g. from a
 * layer's update(dt)) without the cost bgem_text_setString() has.
 *
 * No-ops with a DEBUG_PRINT if `id` is out of range or has no object.
 */
void bgem_text_editParam(int id, bgem_text_param param, float value);

/**
 * @brief
 * Draws one text object into whichever FBO is currently bound.
 *
 * There is no implicit "draw everything" pass - each layer's render()
 * calls this explicitly for whichever ids belong to it. Which compositor
 * layer a text object visually ends up in is entirely determined by
 * *when* this is called during the frame, exactly like every other
 * layer's render() function determines its own output by when the
 * compositor calls it.
 */
void bgem_text_render(int id);

/**
 * @brief
 * Frees a text object's GL resources and clears its slot for reuse.
 *
 * Only needed when an object should stop existing entirely. For text
 * that just changes content, use bgem_text_setString() instead - it's
 * cheaper and keeps the same slot.
 */
void bgem_text_removeText(int id);

#endif /* TEXT_H */
