/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <math.h>

#include "layers/ui.h"
#include "text/text.h"

/* Slot ids for this prototype's two test labels. Plain local constants -
 * ids only need to be unique within whichever layers actually call
 * bgem_text_addText(), and right now that's just this one. */
#define TEXT_ID_HELLO 0
#define TEXT_ID_WORLD 1

/* Rough text-rendering prototype: registers a couple of test text
 * objects (see text/text.h). Not the real UI API framework - just
 * confirming the FreeType/Harfbuzz pipeline and the UI layer's place in
 * the compositor both work.
 */
bgem_result bgem_ui_init(void)
{
    bgem_result status;

    status = bgem_text_addText(TEXT_ID_HELLO, 60.0f, 100.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, "Hello");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(TEXT_ID_WORLD, 60.0f, 150.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, "world!");
    if (status != BGEM_OK) return status;

    return BGEM_OK;
}

void bgem_ui_destroy(void)
{
    bgem_text_removeText(TEXT_ID_HELLO);
    bgem_text_removeText(TEXT_ID_WORLD);
}

void bgem_ui_update(float dt)
{
    /*
     * Rough smoke test for bgem_text_editParam() - cycles TEXT_ID_WORLD's
     * color so the edit path is visibly exercised without wiring up
     * input yet. Not part of the real UI API, just confirming the
     * cheap-uniform-edit path actually works end to end.
     *
     * Accumulating by dt (not a fixed per-frame step) keeps the cycle
     * speed consistent regardless of frame rate.
     */
    static float t = 0.0f;
    t += dt;
    float r = 0.5f + 0.5f * sinf(t);
    float g = 0.5f + 0.5f * sinf(t + 2.094f); /* +120 deg */
    float b = 0.5f + 0.5f * sinf(t + 4.189f); /* +240 deg */
    bgem_text_editParam(TEXT_ID_WORLD, BGEM_TEXT_PARAM_RED,   r);
    bgem_text_editParam(TEXT_ID_WORLD, BGEM_TEXT_PARAM_GREEN, g);
    bgem_text_editParam(TEXT_ID_WORLD, BGEM_TEXT_PARAM_BLUE,  b);
    (void)dt;
}

void bgem_ui_render(void)
{
    bgem_text_render(TEXT_ID_HELLO);
    bgem_text_render(TEXT_ID_WORLD);
}
