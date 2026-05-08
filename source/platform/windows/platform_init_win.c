/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform/platform_window.h"
#include "platform/platform_init.h"
#include "core/debug.h"

#define BGEM_INSTANCE_MUTEX_NAME L"Local\\bgem_instance_lock"

static HANDLE s_instanceMutex = NULL;

bgem_instance_lockStatus bgem_platform_instanceLock(void)
{
    s_instanceMutex = CreateMutexW(NULL, FALSE, BGEM_INSTANCE_MUTEX_NAME);
    if (s_instanceMutex == NULL)
    {
        return BGEM_INSTANCE_LOCK_ERROR;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        DEBUG_PRINT("There's already and instance opened!");
        CloseHandle(s_instanceMutex);
        s_instanceMutex = NULL;
        bgem_platform_windowBringForward();
        return BGEM_INSTANCE_LOCK_ALREADY_RUNNING;
    }

    return BGEM_INSTANCE_LOCK_OK;

}

void bgem_platform_instanceUnlock(void)
{
    if (s_instanceMutex != NULL) {
        ReleaseMutex(s_instanceMutex);
        CloseHandle(s_instanceMutex);
        s_instanceMutex = NULL;
    }
}
