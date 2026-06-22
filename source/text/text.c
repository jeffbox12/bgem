/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <GLES3/gl3.h>
#include <hb.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "text/text.h"
#include "text/font.h"
#include "shader/shader.h"
#include "core/debug.h"

typedef struct {
    bool   in_use;
    GLuint vbo;
    int    vertex_count;
    float  pos[2];
    float  color[4];
} bgem_text_object;

static bgem_text_object s_objects[BGEM_TEXT_MAX];

static GLuint s_program;
static GLint  s_pos_loc;
static GLint  s_uv_loc;
static GLint  s_position_loc;
static GLint  s_color_loc;
static GLint  s_screenSize_loc;
static GLint  s_atlas_loc;

static float s_screenWidth;
static float s_screenHeight;

static bool isValidId(int id)
{
    if (id < 0 || id >= BGEM_TEXT_MAX)
    {
        DEBUG_PRINT("text: id %d out of range [0, %d)", id, BGEM_TEXT_MAX);
        return false;
    }
    return true;
}

/* Frees an object's GL resources without touching in_use - shared by
 * removeText() and by addText() when overwriting an already-used slot. */
static void freeObjectResources(bgem_text_object *obj)
{
    if (obj->vbo) { glDeleteBuffers(1, &obj->vbo); obj->vbo = 0; }
    obj->vertex_count = 0;
}

/*
 * Shapes `str` via Harfbuzz and walks the result into an interleaved
 * (x, y, u, v) vertex buffer, two triangles per glyph, unindexed (the
 * simplest thing that works for a handful of short test strings).
 *
 * Geometry is local to the object's own pen origin (0,0) at the top-left
 * of its line box - bgem_text_render() adds the object's uPosition
 * uniform on the GPU side, so moving an object via editParam() never
 * touches this buffer.
 */
static bgem_result buildGeometry(bgem_text_object *obj, const char *str)
{
    hb_glyph_info_t     *infos;
    hb_glyph_position_t *positions;
    unsigned int count = bgem_font_shapeText(str, &infos, &positions);

    if (count == 0)
    {
        /* Empty string - valid, just nothing to draw. */
        freeObjectResources(obj);
        return BGEM_OK;
    }

    float *verts = malloc(sizeof(float) * 4 * 6 * count);
    if (!verts)
    {
        DEBUG_PRINT("buildGeometry: out of memory for %u glyphs", count);
        return BGEM_ERROR_OOM;
    }

    float pen_x    = 0.0f;
    float pen_y    = 0.0f;
    float ascender = bgem_font_getAscender();
    int   written  = 0;

    for (unsigned int i = 0; i < count; i++)
    {
        const bgem_font_glyphInfo *g = bgem_font_getGlyphInfo(infos[i].codepoint);

        float advance_x = (float)positions[i].x_advance / 64.0f;
        float advance_y = (float)positions[i].y_advance / 64.0f;
        float offset_x  = (float)positions[i].x_offset  / 64.0f;
        float offset_y  = (float)positions[i].y_offset  / 64.0f;

        if (g && g->width > 0.0f && g->height > 0.0f)
        {
            /* x: pen + shaping offset + how far the bitmap sits right of
             *    the pen (bearing_x).
             * y: ascender (line-top -> baseline) minus how far the
             *    bitmap sits above the baseline (bearing_y), since local
             *    space is Y-down but FreeType's bearing points up. */
            float x0 = pen_x + offset_x + g->bearing_x;
            float y0 = pen_y + ascender - g->bearing_y - offset_y;
            float x1 = x0 + g->width;
            float y1 = y0 + g->height;

            float *v = &verts[written * 4 * 6];
            float quad[6][4] = {
                { x0, y0, g->u0, g->v0 },
                { x1, y0, g->u1, g->v0 },
                { x1, y1, g->u1, g->v1 },
                { x0, y0, g->u0, g->v0 },
                { x1, y1, g->u1, g->v1 },
                { x0, y1, g->u0, g->v1 },
            };
            memcpy(v, quad, sizeof(quad));
            written++;
        }
        else if (!g)
        {
            DEBUG_PRINT("buildGeometry: glyph index %u not in atlas, skipping",
                        (unsigned int)infos[i].codepoint);
        }

        pen_x += advance_x;
        pen_y += advance_y;
    }

    freeObjectResources(obj);

    if (written > 0)
    {
        glGenBuffers(1, &obj->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6 * (size_t)written, verts, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    obj->vertex_count = written * 6;

    free(verts);
    return BGEM_OK;
}

bgem_result bgem_text_init(int screen_width, int screen_height)
{
    bgem_result status = bgem_font_init();
    if (status != BGEM_OK) return status;

    s_program = bgem_shader_get("text");
    if (!s_program)
    {
        DEBUG_PRINT("bgem_text_init: shader 'text' not found");
        bgem_font_destroy();
        return BGEM_ERROR_IO;
    }

    s_pos_loc        = glGetAttribLocation(s_program,  "aPos");
    s_uv_loc         = glGetAttribLocation(s_program,  "aUV");
    s_position_loc   = glGetUniformLocation(s_program, "uPosition");
    s_color_loc      = glGetUniformLocation(s_program, "uColor");
    s_screenSize_loc = glGetUniformLocation(s_program, "uScreenSize");
    s_atlas_loc      = glGetUniformLocation(s_program, "uAtlas");

    s_screenWidth  = (float)screen_width;
    s_screenHeight = (float)screen_height;

    memset(s_objects, 0, sizeof(s_objects));

    return BGEM_OK;
}

void bgem_text_shutdown(void)
{
    for (int i = 0; i < BGEM_TEXT_MAX; i++)
    {
        if (s_objects[i].in_use)
        {
            freeObjectResources(&s_objects[i]);
            s_objects[i].in_use = false;
        }
    }
    bgem_font_destroy();
}

bgem_result bgem_text_addText(int id, float posX, float posY,
                               float r, float g, float b, float a,
                               const char *str)
{
    if (!isValidId(id)) return BGEM_ERROR_IO;

    bgem_text_object *obj = &s_objects[id];

    if (obj->in_use)
    {
        DEBUG_PRINT("bgem_text_addText: id %d already in use, rebuilding in place", id);
        freeObjectResources(obj);
    }

    obj->pos[0]   = posX;
    obj->pos[1]   = posY;
    obj->color[0] = r;
    obj->color[1] = g;
    obj->color[2] = b;
    obj->color[3] = a;

    bgem_result status = buildGeometry(obj, str);
    if (status != BGEM_OK) return status;

    obj->in_use = true;
    return BGEM_OK;
}

bgem_result bgem_text_setString(int id, const char *str)
{
    if (!isValidId(id)) return BGEM_ERROR_IO;

    bgem_text_object *obj = &s_objects[id];
    if (!obj->in_use)
    {
        DEBUG_PRINT("bgem_text_setString: id %d has no object, call addText first", id);
        return BGEM_ERROR_NOT_FOUND;
    }

    return buildGeometry(obj, str);
}

void bgem_text_editParam(int id, bgem_text_param param, float value)
{
    if (!isValidId(id)) return;

    bgem_text_object *obj = &s_objects[id];
    if (!obj->in_use)
    {
        DEBUG_PRINT("bgem_text_editParam: id %d has no object", id);
        return;
    }

    switch (param)
    {
        case BGEM_TEXT_PARAM_POSX:  obj->pos[0]   = value; break;
        case BGEM_TEXT_PARAM_POSY:  obj->pos[1]   = value; break;
        case BGEM_TEXT_PARAM_RED:   obj->color[0] = value; break;
        case BGEM_TEXT_PARAM_GREEN: obj->color[1] = value; break;
        case BGEM_TEXT_PARAM_BLUE:  obj->color[2] = value; break;
        case BGEM_TEXT_PARAM_ALPHA: obj->color[3] = value; break;
        default: DEBUG_PRINT("bgem_text_editParam: unknown param %d", (int)param); break;
    }
}

void bgem_text_render(int id)
{
    if (!isValidId(id)) return;

    bgem_text_object *obj = &s_objects[id];
    if (!obj->in_use || obj->vertex_count == 0) return;

    glUseProgram(s_program);

    glUniform2f(s_position_loc, obj->pos[0], obj->pos[1]);
    glUniform4f(s_color_loc, obj->color[0], obj->color[1], obj->color[2], obj->color[3]);
    glUniform2f(s_screenSize_loc, s_screenWidth, s_screenHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgem_font_getAtlasTexture());
    glUniform1i(s_atlas_loc, 0);

    /* First real use of GL_BLEND in bgem's own rendering code. Glyph
     * edges are antialiased and adjacent quads can overlap slightly, so
     * an unblended draw would show seams at glyph boundaries. Scoped
     * tightly around this draw and disabled again immediately after -
     * nothing else in the compositor resets blend state between layers,
     * so leaving it enabled would silently affect whatever renders next. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);

    glEnableVertexAttribArray((GLuint)s_pos_loc);
    glVertexAttribPointer((GLuint)s_pos_loc, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void *)0);

    glEnableVertexAttribArray((GLuint)s_uv_loc);
    glVertexAttribPointer((GLuint)s_uv_loc, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void *)(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, obj->vertex_count);

    glDisableVertexAttribArray((GLuint)s_pos_loc);
    glDisableVertexAttribArray((GLuint)s_uv_loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
}

void bgem_text_removeText(int id)
{
    if (!isValidId(id)) return;

    bgem_text_object *obj = &s_objects[id];
    if (!obj->in_use) return;

    freeObjectResources(obj);
    obj->in_use = false;
}
