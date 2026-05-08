/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <windows.h>

#include "platform/platform_window.h"
#include "bgem.h"

void bgem_platform_windowBringForward(void)
{
    HWND hwnd = FindWindowW(NULL, BGEM_APP_NAME_W);
        if (hwnd) {
            if (IsIconic(hwnd)) {
                ShowWindow(hwnd, SW_RESTORE);
            }
            SetForegroundWindow(hwnd);
        }
}
