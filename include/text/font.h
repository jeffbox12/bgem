/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef FONT_H
#define FONT_H

#include <GLES3/gl3.h>
#include <hb.h>

#include "core/debug.h" /* bgem_result */

/**
 * @brief
 * Printable ASCII range baked into the shared glyph atlas.
 *
 * Good enough for a rough prototype confirming the FreeType/Harfbuzz
 * pipeline works. Real Unicode coverage (and a real atlas packer) is a
 * problem for the eventual UI API, not this test.
 */
#define BGEM_FONT_GLYPH_MIN   32  /* ' ' */
#define BGEM_FONT_GLYPH_MAX   126 /* '~' */
#define BGEM_FONT_GLYPH_COUNT (BGEM_FONT_GLYPH_MAX - BGEM_FONT_GLYPH_MIN + 1)

/**
 * @brief
 * Baked glyph height, in pixels.
 *
 * Passed directly to FT_Set_Pixel_Sizes(). At the 72dpi FreeType assumes
 * when no DPI is given, a point size and a pixel size are the same
 * number, so this doubles as "48pt" against bgem's fixed 1920x1080
 * internal canvas - there's no separate DPI conversion happening here.
 */
#define BGEM_FONT_PIXEL_SIZE 48

/**
 * @brief
 * Baked metrics and atlas location for a single rasterized glyph.
 *
 * Keyed by FreeType glyph index, NOT by ASCII/Unicode codepoint. After
 * shaping, Harfbuzz hands back glyph indices - font-internal IDs - which
 * are not guaranteed to line up with character codes. Confirmed with a
 * real font during development: shaping "Hello" with DejaVu Sans returns
 * glyph indices {43, 72, 79, 79, 82} for H/e/l/l/o, none of which match
 * their ASCII values. The atlas is baked and looked up by glyph index
 * for exactly this reason.
 */
typedef struct {
    unsigned int glyph_index;
    float u0, v0, u1, v1;       /**< Atlas UV rect, [0,1] */
    float width, height;        /**< Bitmap size, in pixels */
    float bearing_x, bearing_y; /**< FreeType bitmap_left/bitmap_top, in pixels */
} bgem_font_glyphInfo;

/**
 * @brief
 * Initializes FreeType and Harfbuzz, loads the bundled font face, and
 * bakes a single shared glyph atlas covering BGEM_FONT_GLYPH_MIN..MAX.
 *
 * Requires a current GL context (the atlas is uploaded to a GL texture
 * as part of this call).
 *
 * @return BGEM_OK on success, BGEM_ERROR_IO on a FreeType/Harfbuzz/file
 * failure, BGEM_ERROR_OOM if a host allocation fails while baking.
 */
bgem_result bgem_font_init(void);

/** Releases FreeType, Harfbuzz, and the atlas texture. */
void bgem_font_destroy(void);

/**
 * @brief
 * Shapes a UTF-8 string with Harfbuzz.
 *
 * @param[in] str
 * Null-terminated UTF-8 string to shape.
 *
 * @param[out] out_glyphInfos
 * Receives a pointer to the shaped hb_glyph_info_t array. Element
 * [i].codepoint is the glyph index for the i-th shaped glyph - feed it
 * to bgem_font_getGlyphInfo(), not the original character code.
 *
 * @param[out] out_glyphPositions
 * Receives a pointer to the shaped hb_glyph_position_t array, parallel
 * to out_glyphInfos. x_advance/y_advance/x_offset/y_offset are all in
 * 26.6 fixed point (divide by 64.0 for pixels).
 *
 * @return Number of shaped glyphs (length of both output arrays).
 *
 * @note
 * Both arrays are owned by this module's internal scratch hb_buffer_t
 * and are only valid until the next call to bgem_font_shapeText(). Don't
 * hold onto them across calls.
 */
unsigned int bgem_font_shapeText(const char *str,
                                  hb_glyph_info_t **out_glyphInfos,
                                  hb_glyph_position_t **out_glyphPositions);

/**
 * @brief
 * Looks up a baked glyph's atlas metrics by FreeType glyph index.
 *
 * @param[in] glyph_index
 * A glyph index, typically taken from a shaped hb_glyph_info_t.codepoint.
 *
 * @return
 * Pointer to the glyph's baked info, or NULL if that glyph index wasn't
 * baked into the atlas (e.g. a character outside the printable ASCII
 * range this prototype supports).
 */
const bgem_font_glyphInfo *bgem_font_getGlyphInfo(unsigned int glyph_index);

/** @return The font's ascender, in pixels, at BGEM_FONT_PIXEL_SIZE. */
float bgem_font_getAscender(void);

/** @return The GL texture handle of the baked glyph atlas (single-channel, GL_R8). */
GLuint bgem_font_getAtlasTexture(void);

#endif /* FONT_H */
