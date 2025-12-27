/*
 * File: src/c_modules/timer_logic.c
 * Description: Implementation of periodic logic.
 */

#include "timer_logic.h"

// --- Private State ---
static volatile bool heartbeat_state = false;
static volatile bool heartbeat_enabled = true;

// --- Setter ---
void timer_core_set_heartbeat_enable(bool enabled) {
    heartbeat_enabled = enabled;
}

bool timer_core_toggle_heartbeat(void) {
    if (!heartbeat_enabled) {
        return heartbeat_state;
    }

    heartbeat_state = !heartbeat_state;
    return heartbeat_state;
}

// --- Getter ---
bool timer_core_is_heartbeat_enabled(void) {
    return heartbeat_enabled;
}