/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef SYSTEM_H
#define SYSTEM_H

/**
 * @brief
 * Initializes and runs the program.
 *
 * Initialization routines are performed, like checking closure status to decide
 * wheither to run repair and diagnostics routines. Initialize the graphics subsystem
 * and check compatibility. Read and fill in current host system information settings.
 * And execute the main application program loop.
 *
 * @return
 * EXIT_SUCCESS is returned when all clean up and closing
 * procedures are executed without problems. EXIT_FAILURE is returned when a function,
 * in the middle of execution, did not meet the expected requirement or a failure has
 * occurred.
 */
int bgem_system_start(void);

#endif /* SYSTEM_H */
