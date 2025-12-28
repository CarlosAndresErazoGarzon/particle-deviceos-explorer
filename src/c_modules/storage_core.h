/*
 * File: src/c_modules/storage_core.h
 * Description: Interface for low-level file system operations (LittleFS).
 */

#ifndef STORAGE_CORE_H
#define STORAGE_CORE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// System Snapshot
typedef struct {
    int countA;
    int countB;
    int countC;
    uint32_t magic_number; 
} AppState;

// --- IO Functions ---

// Saves the current state
bool storage_save_state(const AppState* state);
// Loads the state from memory
bool storage_load_state(AppState* state);
// Resets data from memory
bool storage_reset_data(void);
// Checks the Bin file
bool storage_file_exists(void);

#ifdef __cplusplus
}
#endif

#endif // STORAGE_CORE_H