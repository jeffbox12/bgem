/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct bgem_config {
    int frame_limit;
    int render_width;
    int render_height;
    bool fullscreen;
} bgem_config;


/**
 * @brief
 * WIP: Load configuration data
 *
 * Right now, it only loads the defined defaults in
 * bgem_defaults.h. Later implementation will read from
 * a user config file, along with checks.
 *
 * @return
 * Stores current configuration data into a struct
 */
bgem_config bgem_config_load(void); // stub for now

#endif /* CONFIG_H */
