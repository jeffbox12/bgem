/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <stdlib.h>
#include <stdbool.h>

#ifndef PATH_H
#define PATH_H

/**
 * @brief
 * Converts the relative path into full path
 *
 * @param[out] out
 * Where to store the full path
 *
 * @param[in] size
 * Size of the character buffer where to store the
 * full path
 *
 * @param[in] relative_path
 * The relative path
 *
 * @return
 * FALSE if the output is bigger than the buffer (size).
 * TRUE if the output fits in the buffer.
 */
bool bgem_path_relativeToFull(char *out, size_t size, const char *relative_path);

#endif /* PATH_H */
