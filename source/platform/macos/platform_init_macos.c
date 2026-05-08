/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include "platform/platform_init.h"

bgem_instance_lockStatus bgem_platform_instanceLock(void)
{
    // STUB: Instance locking is handled by Launch Services
    // TODO: However, launching the raw executable within the bundle
    //       will open a new instance.
    return BGEM_INSTANCE_LOCK_OK;
}

void bgem_platform_instanceUnlock(void)
{
    // STUB: Handled by Launch Services
}
