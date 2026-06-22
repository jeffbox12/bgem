/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <math.h>

#include "layers/ui.h"
#include "text/text.h"
#include "input/input.h"
#include "bgem_version.h"

typedef enum {
    TITLE_HEADER = 0,
    CONTROLS_FOOTER,
    PROTOTYPE_LABEL,
    STATIC_ID_COUNTER
} text_id_static;

typedef enum {
    ITEM_ONE = STATIC_ID_COUNTER,
    ITEM_TWO,
    ITEM_THREE,
    ITEM_QUIT,
    HOMEPAGE_ID_COUNTER
} text_id_homepage;

/*
 * Subpage IDs alias each other at HOMEPAGE_ID_COUNTER because each enum
 * chains from the same base. This is safe: only one subpage is ever
 * registered in the text system at a time, enforced by destroy-before-init
 * in every page transition.
 */
typedef enum {
    HELLO_TEXT = HOMEPAGE_ID_COUNTER,
    HELLOPAGE_ID_COUNTER
} text_id_hellopage;

typedef enum {
    DEBUG_TEXT = HOMEPAGE_ID_COUNTER,
    DEBUGPAGE_ID_COUNTER
} text_id_debugpage;

typedef enum {
    ABOUT_TEXT = DEBUGPAGE_ID_COUNTER,
    ABOUT_VERSION_STRING,
    ABOUTPAGE_ID_COUNTER
} text_id_aboutpage;

typedef enum {
    PAGE_HOME,
    PAGE_HELLO,
    PAGE_DEBUG,
    PAGE_ABOUT,
} ui_page;

static ui_page current_page = PAGE_HOME;

#define HOMEPAGE_ITEM_COUNT (HOMEPAGE_ID_COUNTER - STATIC_ID_COUNTER)
static int homepage_items[HOMEPAGE_ITEM_COUNT] = {
    ITEM_ONE, ITEM_TWO, ITEM_THREE, ITEM_QUIT
};

typedef struct homepage_selection {
    int *items;
    int  selected;
} homepage_selection;
static homepage_selection hs;

/* Static text (always visible) ------------------------------- */
bgem_result init_headerFooter(void)
{
    bgem_result status = BGEM_ERROR_IO;

    status = bgem_text_addText(TITLE_HEADER,
                                60.0f, 100.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "interactive UI test");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(CONTROLS_FOOTER,
                                60.0f, 1000.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "Arrow keys - Move    Enter - OK     Esc - Back");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(PROTOTYPE_LABEL,
                                1600.0f, 0.0f,
                                1.0f, 1.0f, 1.0f, 0.85f,
                                "PROTOTYPE");
    if (status != BGEM_OK) return status;

    return status;
}

void destroy_headerFooter(void)
{
    bgem_text_removeText(TITLE_HEADER);
    bgem_text_removeText(CONTROLS_FOOTER);
    bgem_text_removeText(PROTOTYPE_LABEL);
}

void render_headerFooter(void)
{
    bgem_text_render(TITLE_HEADER);
    bgem_text_render(CONTROLS_FOOTER);
    bgem_text_render(PROTOTYPE_LABEL);
}

/* Hello page ------------------------------- */
bgem_result init_helloPage(void)
{
    bgem_result status = bgem_text_addText(HELLO_TEXT,
                                            100.0f, 200.0f,
                                            1.0f, 1.0f, 1.0f, 1.0f,
                                            "Hello Bluegem!");
    return status;
}

void destroy_helloPage(void)
{
    bgem_text_removeText(HELLO_TEXT);
}

void render_helloPage(void)
{
    bgem_text_render(HELLO_TEXT);
}

/* Debug page ------------------------------- */
bgem_result init_debugPage(void)
{
    bgem_result status = bgem_text_addText(DEBUG_TEXT,
                                            100.0f, 200.0f,
                                            1.0f, 1.0f, 1.0f, 1.0f,
                                            "Unimplemented");
    return status;
}

void destroy_debugPage(void)
{
    bgem_text_removeText(DEBUG_TEXT);
}

void render_debugPage(void)
{
    bgem_text_render(DEBUG_TEXT);
}

/* About page ------------------------------- */
bgem_result init_aboutPage(void)
{
    bgem_result status = bgem_text_addText(ABOUT_TEXT,
                                            100.0f, 200.0f,
                                            1.0f, 1.0f, 1.0f, 1.0f,
                                            "About Bluegem");
    status = bgem_text_addText(ABOUT_VERSION_STRING,
                                            100.0f, 250.0f,
                                            1.0f, 1.0f, 1.0f, 1.0f,
                                            BGEM_VERSION_STRING);
    return status;
}

void destroy_aboutPage(void)
{
    bgem_text_removeText(ABOUT_TEXT);
    bgem_text_removeText(ABOUT_VERSION_STRING);
}

void render_aboutPage(void)
{
    bgem_text_render(ABOUT_TEXT);
    bgem_text_render(ABOUT_VERSION_STRING);
}

/* Home page ------------------------------- */
bgem_result init_homePage(void)
{
    bgem_result status = BGEM_ERROR_IO;

    status = bgem_text_addText(ITEM_ONE,
                                100.0f, 200.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "Hello");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(ITEM_TWO,
                                100.0f, 250.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "Debug info");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(ITEM_THREE,
                                100.0f, 300.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "About");
    if (status != BGEM_OK) return status;

    status = bgem_text_addText(ITEM_QUIT,
                                100.0f, 400.0f,
                                1.0f, 1.0f, 1.0f, 1.0f,
                                "Quit");
    if (status != BGEM_OK) return status;

    hs.items    = homepage_items;
    hs.selected = 0;
    bgem_text_editParam(hs.items[0], BGEM_TEXT_PARAM_BLUE, 0.0f); /* Default selection */

    return status;
}

void destroy_homePage(void)
{
    bgem_text_removeText(ITEM_ONE);
    bgem_text_removeText(ITEM_TWO);
    bgem_text_removeText(ITEM_THREE);
    bgem_text_removeText(ITEM_QUIT);
}

void render_homePage(void)
{
    bgem_text_render(ITEM_ONE);
    bgem_text_render(ITEM_TWO);
    bgem_text_render(ITEM_THREE);
    bgem_text_render(ITEM_QUIT);
}

static void navigate_homePage(bgem_action action)
{
    int prev = hs.selected;

    switch (action)
    {
        case BGEM_ACTION_NAVIGATE_DOWN:
            hs.selected = (prev + 1) % HOMEPAGE_ITEM_COUNT;
            break;
        case BGEM_ACTION_NAVIGATE_UP:
            hs.selected = (prev - 1 + HOMEPAGE_ITEM_COUNT) % HOMEPAGE_ITEM_COUNT;
            break;
        default:
            return;
    }

    bgem_text_editParam(hs.items[prev],        BGEM_TEXT_PARAM_BLUE, 1.0f); /* deselect */
    bgem_text_editParam(hs.items[hs.selected], BGEM_TEXT_PARAM_BLUE, 0.0f); /* select   */
}

/* Page transitions ------------------------------- */
static bgem_result confirm_homePage(void)
{
    if (hs.selected == 3) /* Quit: no page transition */
    {
        /* TODO: signal application quit */
        DEBUG_PRINT("Unimplemented. Manually quit the program.");
        return BGEM_OK;
    }

    destroy_homePage();

    bgem_result status = BGEM_OK;
    switch (hs.selected)
    {
        case 0: status = init_helloPage(); if (status == BGEM_OK) current_page = PAGE_HELLO; break;
        case 1: status = init_debugPage(); if (status == BGEM_OK) current_page = PAGE_DEBUG; break;
        case 2: status = init_aboutPage(); if (status == BGEM_OK) current_page = PAGE_ABOUT; break;
        default: status = BGEM_ERROR_IO; break;
    }

    if (status != BGEM_OK)
    {
        /* Subpage init failed; recover by returning to home rather than
         * leaving the UI in a half-initialized state.
         */
        init_homePage();
    }

    return status;
}

static bgem_result back_toHomePage(void)
{
    switch (current_page)
    {
        case PAGE_HELLO: destroy_helloPage(); break;
        case PAGE_DEBUG: destroy_debugPage(); break;
        case PAGE_ABOUT: destroy_aboutPage(); break;
        default: return BGEM_OK; /* already home */
    }

    bgem_result status = init_homePage();
    if (status != BGEM_OK) return status;

    current_page = PAGE_HOME;
    return BGEM_OK;
}

/* Rough text-rendering prototype: registers a couple of test text
 * objects (see text/text.h). Not the real UI API framework - just
 * confirming the FreeType/Harfbuzz pipeline and the UI layer's place in
 * the compositor both work.
 */
bgem_result bgem_ui_init(void)
{
    bgem_result status;
    status = init_headerFooter();
    if (status != BGEM_OK) return status;
    status = init_homePage();
    if (status != BGEM_OK) return status;

    current_page = PAGE_HOME;
    return status;
}

void bgem_ui_destroy(void)
{
    destroy_headerFooter();
    switch (current_page)
    {
        case PAGE_HOME:  destroy_homePage();  break;
        case PAGE_HELLO: destroy_helloPage(); break;
        case PAGE_DEBUG: destroy_debugPage(); break;
        case PAGE_ABOUT: destroy_aboutPage(); break;
        default: break;
    }
}

void bgem_ui_update(float dt)
{
    /*
     * Rough menu navigation. This is for testing the input interactivity
     * with a basic UI.
     */
    bgem_action action = BGEM_ACTION_COUNT; /* TODO: Used as "no action". There are fundamental missing features
                                             * with the input system that should be revised.
                                             */
    if (bgem_input_actionJustPressed(BGEM_ACTION_NAVIGATE_DOWN)) action = BGEM_ACTION_NAVIGATE_DOWN;
    if (bgem_input_actionJustPressed(BGEM_ACTION_NAVIGATE_UP))   action = BGEM_ACTION_NAVIGATE_UP;
    if (bgem_input_actionJustPressed(BGEM_ACTION_CONFIRM))       action = BGEM_ACTION_CONFIRM;
    if (bgem_input_actionJustPressed(BGEM_ACTION_BACK))          action = BGEM_ACTION_BACK;

    switch (current_page)
    {
        case PAGE_HOME:
            if (action == BGEM_ACTION_CONFIRM)
                confirm_homePage();
            else
                navigate_homePage(action);
            break;
        case PAGE_HELLO:
        case PAGE_DEBUG:
        case PAGE_ABOUT:
            if (action == BGEM_ACTION_BACK)
                back_toHomePage();
            break;
        default:
            break;
    }

    (void)dt;
}

void bgem_ui_render(void)
{
    render_headerFooter();
    switch (current_page)
    {
        case PAGE_HOME:  render_homePage();  break;
        case PAGE_HELLO: render_helloPage(); break;
        case PAGE_DEBUG: render_debugPage(); break;
        case PAGE_ABOUT: render_aboutPage(); break;
        default: break;
    }
}
