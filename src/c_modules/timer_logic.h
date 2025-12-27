/*
 * File: src/c_modules/timer_logic.h
 * Description: Logic for periodic tasks (Software Timers).
 */

#ifndef TIMER_LOGIC_H
#define TIMER_LOGIC_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Heartbeat Logic ---
bool timer_core_toggle_heartbeat(void);

#ifdef __cplusplus
}
#endif

#endif // TIMER_LOGIC_H