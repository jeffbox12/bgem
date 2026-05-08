/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef PLATFORM_INIT_H
#define PLATFORM_INIT_H

typedef enum {
    BGEM_INSTANCE_LOCK_OK,
    BGEM_INSTANCE_LOCK_ALREADY_RUNNING,
    BGEM_INSTANCE_LOCK_ERROR,
} bgem_instance_lockStatus;

bgem_instance_lockStatus bgem_platform_instanceLock(void);
void bgem_platform_instanceUnlock(void);

#endif // PLATFORM_INIT_H
