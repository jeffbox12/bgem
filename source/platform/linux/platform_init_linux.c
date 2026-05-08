/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "platform/platform_init.h"
#include "platform/platform_window.h"
#include "core/debug.h"

static int sLockFd = -1;
static char sLockPath[64];

bgem_instance_lockStatus bgem_platform_instanceLock(void)
{
    int len;
    int err;

    // /tmp/bgem-<uid>.lock keeps instances per-user isolated
    len = snprintf(sLockPath, sizeof(sLockPath), "/tmp/bgem-%d.lock", (int)getuid());
    if (len < 0 || len >= (int)sizeof(sLockPath)) { DEBUG_PRINT("Path length exceeded!"); return BGEM_INSTANCE_LOCK_ERROR; }

    sLockFd = open(sLockPath, O_CREAT | O_RDWR, 0600);
    if (sLockFd < 0) return BGEM_INSTANCE_LOCK_ERROR;

    if (flock(sLockFd, LOCK_EX | LOCK_NB) < 0) {
        err = errno;
        if (err == EWOULDBLOCK) {
            close(sLockFd);
            DEBUG_PRINT("There's already an instance opened!");
            bgem_platform_windowBringForward();
            return BGEM_INSTANCE_LOCK_ALREADY_RUNNING; // Another instance is running
        }
        close(sLockFd);
        return BGEM_INSTANCE_LOCK_ERROR;
    }

    return BGEM_INSTANCE_LOCK_OK; // Caller keeps fd open until exit
}

void bgem_platform_instanceUnlock(void)
{
    if (sLockFd >= 0) {
        close(sLockFd);
        sLockFd = -1;
    }
    remove(sLockPath);
}
