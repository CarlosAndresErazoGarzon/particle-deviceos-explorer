/*
 * File: src/c_modules/gpio_core.c
 * Description: Pure C Logic Implementation.
 */

#include "gpio_core.h"

// --- Private State ---
static volatile int internal_counter_a = 0;
static volatile int internal_counter_b = 0;
static volatile int internal_counter_c = 0;

static volatile bool ui_update_needed = false;

// Timestamps for debounce logic
static volatile uint32_t last_debounce_a = 0;
static volatile uint32_t last_debounce_b = 0;
static volatile uint32_t last_debounce_c = 0;

// --- Configuration ---
#define DEBOUNCE_THRESHOLD_MS 300

// Dependency Injetion
static system_tick_fn get_system_time_ms = 0;

// --- Init ---
void core_init(system_tick_fn time_provider) {
    get_system_time_ms = time_provider;
}

// --- Helper Functions ---
static uint32_t safe_millis(void) {
    if (get_system_time_ms) {
        return get_system_time_ms();
    }
    return 0;
}

// --- Getters Implementation ---
int core_get_counter_a(void) { return internal_counter_a; }
int core_get_counter_b(void) { return internal_counter_b; }
int core_get_counter_c(void) { return internal_counter_c; }

// --- Logic Functions ---
bool core_check_and_clear_ui_flag(void) {
    if (ui_update_needed) {
        ui_update_needed = false;
        return true;
    }
    return false;
}

// --- ISR Implementation ---
void core_isr_btn_a(void) {
    uint32_t now = safe_millis();
    if (now - last_debounce_a > DEBOUNCE_THRESHOLD_MS) {
        internal_counter_a++;
        last_debounce_a = now;
        ui_update_needed = true;
    }
}

void core_isr_btn_b(void) {
    uint32_t now = safe_millis();
    if (now - last_debounce_b > DEBOUNCE_THRESHOLD_MS) {
        internal_counter_b++;
        last_debounce_b = now;
        ui_update_needed = true;
    }
}

void core_isr_btn_c(void) {
    uint32_t now = safe_millis();
    if (now - last_debounce_c > DEBOUNCE_THRESHOLD_MS) {
        internal_counter_c++;
        last_debounce_c = now;
        ui_update_needed = true;
    }
}

// --- Setters ---
void core_set_counters(int a, int b, int c) {
    internal_counter_a = a;
    internal_counter_b = b;
    internal_counter_c = c;
}