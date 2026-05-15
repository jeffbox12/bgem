/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef APP_H
#define APP_H

#include "system/config.h"

/**
 * @brief
 * Run the application.
 *
 * @param[in] cfg
 * Struct containing the settings
 *
 * @return
 * EXIT_SUCCESS if all the shutdown and clean up routines have been
 * completed. EXIT_FAILURE if an error occurred in the middle of
 * execution.
 */
int bgem_app_run(bgem_config* cfg);

#endif /* APP_H */
