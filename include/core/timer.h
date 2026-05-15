/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#ifndef TIMER_H
#define TIMER_H

/**
 * @brief
 * High precision timer in seconds.
 *
 * @return
 * Time elapsed in seconds. Integer part is the whole seconds,
 * and the fractional part are the subsecond reminder.
 */
double bgem_timer_seconds(void);

#endif /* TIMER_H */
