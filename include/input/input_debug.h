/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef INPUT_DEBUG_H
#define INPUT_DEBUG_H

#ifdef DEBUG

#include <stdbool.h>

#include "window/window.h"
#include "core/debug_defaults.h"

/**
 * @brief
 * Activate the debug interface and track input in the logs
 */
void bgem_input_debug(void);

#define BGEM_INPUT_DEBUG() bgem_input_debug()
#define INPUT_DEBUG_PROCESSEVENT(event) if (BGEM_DEBUG_ISACTIVE()) BGEM_DEBUG_PROCESSEVENT(event)

#else

#define BGEM_INPUT_DEBUG() ((void)0)
#define INPUT_DEBUG_PROCESSEVENT(event) ((void)0)

#endif

#endif /* INPUT_DEBUG_H */
