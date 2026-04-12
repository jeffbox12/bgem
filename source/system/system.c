/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>

#include "system/system.h"
#include "app/app.h"
#include "core/debug.h"

int bgem_system_start(void)
{
    DEBUG_PRINT("Hello Bluegem!");

    bgem_app_init();
    return EXIT_SUCCESS;
}
