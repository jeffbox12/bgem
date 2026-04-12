/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

/*
 *  Starting point program execution, initialization, handling and processing
 *  of arguments.
 */

#include <stdlib.h>

#include "system/system.h"
#include "core/debug.h"

int main(int argc, char *argv[])
{
    // Suppress warnings
    // There's no arguments to process yet
    (void)argc;
    (void)argv;

    return bgem_system_start();
}
