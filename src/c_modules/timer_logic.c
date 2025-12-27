/*
 * File: src/c_modules/timer_logic.c
 * Description: Implementation of periodic logic.
 */

#include "timer_logic.h"

// --- Private State ---
static volatile bool heartbeat_state = false;

// --- Implementation ---
bool timer_core_toggle_heartbeat(void) {

    heartbeat_state = !heartbeat_state;

    return heartbeat_state;
}