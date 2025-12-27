/*
 * File: src/c_modules/gpio_core.h
 * Description: Public interface for GPIO logic core.
 */

#ifndef GPIO_CORE_H
#define GPIO_CORE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*system_tick_fn)(void);
void core_init(system_tick_fn time_provider);

// --- Getters ---
int core_get_counter_a(void);
int core_get_counter_b(void);
int core_get_counter_c(void);

// --- UI Flag ---
bool core_check_and_clear_ui_flag(void);

// --- Interrupt Service Routines (ISRs) ---
void core_isr_btn_a(void);
void core_isr_btn_b(void);
void core_isr_btn_c(void);

// Restore internal counters from an external source
void core_set_counters(int a, int b, int c);

#ifdef __cplusplus
}
#endif

#endif // GPIO_CORE_H