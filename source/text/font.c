/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <GLES3/gl3.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "text/font.h"
#include "core/path.h"
#include "core/debug.h"

/* Gap between cells in the atlas grid, in pixels. Avoids GL_LINEAR
 * sampling bleeding a neighboring glyph's pixels in at a quad's edge. */
#define BGEM_FONT_ATLAS_PADDING 2

static FT_Library   s_library;
static FT_Face      s_face;
static hb_font_t    *s_hbFont;
static hb_buffer_t  *s_hbBuffer;

static GLuint s_atlasTexture;
static bgem_font_glyphInfo s_glyphTable[BGEM_FONT_GLYPH_COUNT];
static float s_ascender;

/* One rasterized glyph, held only for the duration of bakeAtlas(). */
typedef struct {
    unsigned int  glyph_index;
    unsigned char *pixels; /* Owned copy, width * height bytes, 8bpp gray. NULL for empty glyphs (e.g. space). */
    int           width;
    int           height;
    int           bearing_x;
    int           bearing_y;
} bgem_font_tempGlyph;

static void freeTempGlyphs(bgem_font_tempGlyph *temp, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(temp[i].pixels);
        temp[i].pixels = NULL;
    }
}

/*
 * Two-pass bake:
 *   Pass 1 - rasterize every glyph in the supported range via FreeType
 *            and copy its bitmap out (FreeType reuses the same bitmap
 *            buffer on every FT_Load_Glyph call, so it can't be kept
 *            past the next iteration). Track the largest glyph seen.
 *   Pass 2 - now that the largest glyph is known, lay out a fixed-size
 *            grid sized to fit it, blit every copied bitmap into its
 *            cell, and record each glyph's UV rect.
 *
 * The fixed-grid approach (every cell sized to the single largest
 * glyph) wastes some atlas space on narrow glyphs like 'i' or '.', but
 * for ~95 glyphs that's a trivial amount of texture memory, and it's a
 * handful of lines instead of a real shelf/skyline packer. Worth
 * revisiting if this ever becomes the real UI text system.
 */
static bgem_result bakeAtlas(void)
{
    bgem_font_tempGlyph temp[BGEM_FONT_GLYPH_COUNT];
    memset(temp, 0, sizeof(temp));

    int max_w = 0;
    int max_h = 0;

    for (int i = 0; i < BGEM_FONT_GLYPH_COUNT; i++)
    {
        unsigned int charcode    = (unsigned int)(BGEM_FONT_GLYPH_MIN + i);
        unsigned int glyph_index = FT_Get_Char_Index(s_face, charcode);

        if (FT_Load_Glyph(s_face, glyph_index, FT_LOAD_RENDER))
        {
            DEBUG_PRINT("bakeAtlas: FT_Load_Glyph failed for charcode %u", charcode);
            freeTempGlyphs(temp, i);
            return BGEM_ERROR_IO;
        }

        FT_GlyphSlot slot = s_face->glyph;
        FT_Bitmap    *bmp = &slot->bitmap;

        temp[i].glyph_index = glyph_index;
        temp[i].width       = (int)bmp->width;
        temp[i].height      = (int)bmp->rows;
        temp[i].bearing_x   = slot->bitmap_left;
        temp[i].bearing_y   = slot->bitmap_top;

        if (temp[i].width > 0 && temp[i].height > 0)
        {
            temp[i].pixels = malloc((size_t)temp[i].width * (size_t)temp[i].height);
            if (!temp[i].pixels)
            {
                DEBUG_PRINT("bakeAtlas: out of memory copying glyph bitmap");
                freeTempGlyphs(temp, i);
                return BGEM_ERROR_OOM;
            }

            /* bitmap.pitch can exceed width (row padding) or be negative
             * (bottom-up storage) - copy row by row to handle both rather
             * than assuming a single memcpy of the whole buffer is safe. */
            int pitch = bmp->pitch;
            const unsigned char *src = bmp->buffer;
            if (pitch < 0) src += (size_t)(-pitch) * (size_t)(bmp->rows - 1);

            for (int row = 0; row < temp[i].height; row++)
            {
                memcpy(temp[i].pixels + row * temp[i].width,
                       src + (long)row * pitch,
                       (size_t)temp[i].width);
            }
        }

        if (temp[i].width  > max_w) max_w = temp[i].width;
        if (temp[i].height > max_h) max_h = temp[i].height;
    }

    int cell_w = max_w + BGEM_FONT_ATLAS_PADDING;
    int cell_h = max_h + BGEM_FONT_ATLAS_PADDING;
    int cols   = (int)ceilf(sqrtf((float)BGEM_FONT_GLYPH_COUNT));
    int rows   = (BGEM_FONT_GLYPH_COUNT + cols - 1) / cols;

    int atlas_w = cols * cell_w;
    int atlas_h = rows * cell_h;

    unsigned char *atlas_pixels = calloc((size_t)atlas_w * (size_t)atlas_h, 1);
    if (!atlas_pixels)
    {
        DEBUG_PRINT("bakeAtlas: out of memory allocating atlas buffer (%dx%d)", atlas_w, atlas_h);
        freeTempGlyphs(temp, BGEM_FONT_GLYPH_COUNT);
        return BGEM_ERROR_OOM;
    }

    for (int i = 0; i < BGEM_FONT_GLYPH_COUNT; i++)
    {
        int col    = i % cols;
        int row    = i / cols;
        int cell_x = col * cell_w;
        int cell_y = row * cell_h;

        if (temp[i].pixels)
        {
            for (int r = 0; r < temp[i].height; r++)
            {
                memcpy(atlas_pixels + (size_t)(cell_y + r) * (size_t)atlas_w + (size_t)cell_x,
                       temp[i].pixels + r * temp[i].width,
                       (size_t)temp[i].width);
            }
        }

        s_glyphTable[i].glyph_index = temp[i].glyph_index;
        s_glyphTable[i].width       = (float)temp[i].width;
        s_glyphTable[i].height      = (float)temp[i].height;
        s_glyphTable[i].bearing_x   = (float)temp[i].bearing_x;
        s_glyphTable[i].bearing_y   = (float)temp[i].bearing_y;
        s_glyphTable[i].u0 = (float)cell_x / (float)atlas_w;
        s_glyphTable[i].v0 = (float)cell_y / (float)atlas_h;
        s_glyphTable[i].u1 = (float)(cell_x + temp[i].width)  / (float)atlas_w;
        s_glyphTable[i].v1 = (float)(cell_y + temp[i].height) / (float)atlas_h;
    }

    freeTempGlyphs(temp, BGEM_FONT_GLYPH_COUNT);

    /* Single-channel (R8) texture - the atlas is a pure coverage mask.
     * Color comes from each text object's uColor uniform at draw time,
     * never from the atlas itself. */
    glGenTextures(1, &s_atlasTexture);
    glBindTexture(GL_TEXTURE_2D, s_atlasTexture);

    /* FreeType's rasterized rows aren't 4-byte aligned by default; without
     * this, glTexImage2D silently reads past row ends on widths that
     * aren't a multiple of 4 and the atlas comes out sheared. Restored
     * to the GL default afterward so it doesn't affect unrelated texture
     * uploads elsewhere later. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlas_w, atlas_h, 0,
                 GL_RED, GL_UNSIGNED_BYTE, atlas_pixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(atlas_pixels);

    DEBUG_PRINT("bakeAtlas: baked %d glyphs into a %dx%d atlas (%d cols x %d rows, cell %dx%d)",
                BGEM_FONT_GLYPH_COUNT, atlas_w, atlas_h, cols, rows, cell_w, cell_h);

    return BGEM_OK;
}

bgem_result bgem_font_init(void)
{
    char fontPath[BGEM_PATH_MAX];
    if (bgem_path_relativeToFull(fontPath, sizeof(fontPath), "assets/font/Inter_18pt-Light.ttf"))
    {
        DEBUG_PRINT("bgem_font_init: could not resolve font path");
        return BGEM_ERROR_IO;
    }

    if (FT_Init_FreeType(&s_library))
    {
        DEBUG_PRINT("bgem_font_init: FT_Init_FreeType failed");
        return BGEM_ERROR_IO;
    }

    if (FT_New_Face(s_library, fontPath, 0, &s_face))
    {
        DEBUG_PRINT("bgem_font_init: FT_New_Face failed for '%s'", fontPath);
        FT_Done_FreeType(s_library);
        s_library = NULL;
        return BGEM_ERROR_IO;
    }

    if (FT_Set_Pixel_Sizes(s_face, 0, BGEM_FONT_PIXEL_SIZE))
    {
        DEBUG_PRINT("bgem_font_init: FT_Set_Pixel_Sizes failed");
        FT_Done_Face(s_face);
        FT_Done_FreeType(s_library);
        s_face = NULL;
        s_library = NULL;
        return BGEM_ERROR_IO;
    }

    s_ascender = (float)(s_face->size->metrics.ascender) / 64.0f;

    s_hbFont = hb_ft_font_create_referenced(s_face);
    if (!s_hbFont)
    {
        DEBUG_PRINT("bgem_font_init: hb_ft_font_create_referenced failed");
        FT_Done_Face(s_face);
        FT_Done_FreeType(s_library);
        s_face = NULL;
        s_library = NULL;
        return BGEM_ERROR_IO;
    }

    s_hbBuffer = hb_buffer_create();
    if (!hb_buffer_allocation_successful(s_hbBuffer))
    {
        DEBUG_PRINT("bgem_font_init: hb_buffer_create failed");
        hb_font_destroy(s_hbFont);
        FT_Done_Face(s_face);
        FT_Done_FreeType(s_library);
        s_hbFont = NULL;
        s_face = NULL;
        s_library = NULL;
        return BGEM_ERROR_OOM;
    }

    bgem_result status = bakeAtlas();
    if (status != BGEM_OK)
    {
        hb_buffer_destroy(s_hbBuffer);
        hb_font_destroy(s_hbFont);
        FT_Done_Face(s_face);
        FT_Done_FreeType(s_library);
        s_hbBuffer = NULL;
        s_hbFont = NULL;
        s_face = NULL;
        s_library = NULL;
        return status;
    }

    DEBUG_PRINT("bgem_font_init: loaded '%s', family '%s', %ld glyphs in face",
                fontPath, s_face->family_name, (long)s_face->num_glyphs);

    return BGEM_OK;
}

void bgem_font_destroy(void)
{
    if (s_atlasTexture) { glDeleteTextures(1, &s_atlasTexture); s_atlasTexture = 0; }
    if (s_hbBuffer)      { hb_buffer_destroy(s_hbBuffer); s_hbBuffer = NULL; }
    if (s_hbFont)        { hb_font_destroy(s_hbFont); s_hbFont = NULL; }
    if (s_face)          { FT_Done_Face(s_face); s_face = NULL; }
    if (s_library)       { FT_Done_FreeType(s_library); s_library = NULL; }
}

unsigned int bgem_font_shapeText(const char *str,
                                  hb_glyph_info_t **out_glyphInfos,
                                  hb_glyph_position_t **out_glyphPositions)
{
    hb_buffer_reset(s_hbBuffer);
    hb_buffer_add_utf8(s_hbBuffer, str, -1, 0, -1);
    hb_buffer_guess_segment_properties(s_hbBuffer);
    hb_shape(s_hbFont, s_hbBuffer, NULL, 0);

    unsigned int count = 0;
    *out_glyphInfos     = hb_buffer_get_glyph_infos(s_hbBuffer, &count);
    *out_glyphPositions = hb_buffer_get_glyph_positions(s_hbBuffer, &count);
    return count;
}

const bgem_font_glyphInfo *bgem_font_getGlyphInfo(unsigned int glyph_index)
{
    for (int i = 0; i < BGEM_FONT_GLYPH_COUNT; i++)
    {
        if (s_glyphTable[i].glyph_index == glyph_index)
            return &s_glyphTable[i];
    }
    return NULL;
}

float bgem_font_getAscender(void)
{
    return s_ascender;
}

GLuint bgem_font_getAtlasTexture(void)
{
    return s_atlasTexture;
}
