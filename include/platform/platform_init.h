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

/**
 * @brief
 * Lock the instance so only one at a time can be opened
 *
 * @return
 * `BGEM_INSTANCE_LOCK_OK` if the lock has been executed successfully.
 * `BGEM_INSTANCE_LOCK_ALREADY_RUNNING` if the new instance detects
 * there is already another one opened, and `BGEM_INSTANCE_LOCK_ERROR`
 * if the instance lock could not be set.
 */
bgem_instance_lockStatus bgem_platform_instanceLock(void);

/**
 * @brief
 * Unlock the instance.
 *
 * This is done during termination and cleanup procedures.
 */
void bgem_platform_instanceUnlock(void);

#endif /* PLATFORM_INIT_H */
