/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

/**
 * @brief
 * Brings the window forward.
 *
 * This only works in Windows. macOS already handles this with Launch
 * Services. Wayland cannot control window status for now.
 *
 * NOTE: This is paired with the instance locking mechanism, hence the
 * MessageBox in Linux will warn about the windows being "already opened"
 * which is not really consistent with what the function is intended to do,
 * at least on Linux
 *
 * TODO: Linux should use a Message Box API instead. Using a function named
 * like that is misleading.
 *
 */
void bgem_platform_windowBringForward(void);

#endif /* PLATFORM_WINDOW_H */
