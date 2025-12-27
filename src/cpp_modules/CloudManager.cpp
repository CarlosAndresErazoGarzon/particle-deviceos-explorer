/*
 * File: src/cpp_libraries/CloudManager.cpp
 * Description: Implementation of Cloud primitives.
 */

#include "CloudManager.h"
#include "../c_modules/gpio_core.h" 
#include "../c_modules/storage_core.h"
#include "../c_modules/timer_logic.h"

static bool _ledState = false;

CloudManager::CloudManager() {
    _cloudCountA = 0;
    _cloudCountB = 0;
    _cloudCountC = 0;
}

void CloudManager::begin() {
    // Push Variables
    Particle.variable("cnt_a", _cloudCountA);
    Particle.variable("cnt_b", _cloudCountB);
    Particle.variable("cnt_c", _cloudCountC);

    // Push Funciones
    Particle.function("reset_all", cloudResetCounters);
    Particle.function("toggle_led", cloudToggleLed);
}

void CloudManager::sync(int a, int b, int c) {
    _cloudCountA = a;
    _cloudCountB = b;
    _cloudCountC = c;
}

// --- Callbacks ---

// RPC: LED Control
int CloudManager::cloudToggleLed(String command) {
    timer_core_set_heartbeat_enable(false);

    if (command == "on") {
        _ledState = true;
    } else if (command == "off") {
        _ledState = false;
    } else {
        _ledState = !_ledState;
    }
    
    digitalWrite(D7, _ledState);
    return _ledState ? 1 : 0;
}

// RPC: Full Reset
int CloudManager::cloudResetCounters(String command) {
    core_set_counters(0, 0, 0);
    storage_reset_data();
    
    timer_core_set_heartbeat_enable(true);
    
    return 1;
}