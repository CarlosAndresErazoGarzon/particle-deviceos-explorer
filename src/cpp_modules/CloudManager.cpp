/*
 * File: src/cpp_modules/CloudManager.cpp
 * Description: Implementation of Cloud primitives.
 */

#include "CloudManager.h"
#include "../c_modules/gpio_core.h" 
#include "../c_modules/storage_core.h"
#include "../c_modules/timer_logic.h"

#include <ArduinoJson.h>

static bool _ledState = false;
const int JSON_CAPACITY = 1024;
char CloudManager::_jsonBuffer[622];


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
    Particle.variable("json_status", _jsonBuffer);

    // --- Push Funciones ---
    // Register Functions
    Particle.function("reset_all", cloudResetCounters);
    Particle.function("toggle_led", cloudToggleLed);

    // File System Functions
    Particle.function("save_data", cloudSaveData);
    Particle.function("del_data", cloudDeleteData);
    Particle.function("chk_data", cloudCheckData);
    Particle.function("scan_i2c", cloudScanI2C);

    // --- Get JSON Status ---
    Particle.function("get_status", cloudGetStatus);

    // Event Subscription
    Particle.subscribe("cmd/status", &CloudManager::handleCloudEvent, this);
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

// RPC: Manual Save
int CloudManager::cloudSaveData(String command) {
    // Gather current state
    AppState currentState;
    currentState.countA = core_get_counter_a();
    currentState.countB = core_get_counter_b();
    currentState.countC = core_get_counter_c();
    currentState.magic_number = 0xCAFEBABE;

    // Write to disk
    bool success = storage_save_state(&currentState);
    
    return success ? 1 : -1;
}

// RPC: Delete File
int CloudManager::cloudDeleteData(String command) {
    return storage_reset_data() ? 1 : 0;
}

// RPC: Check File Exists
int CloudManager::cloudCheckData(String command) {
    return storage_file_exists() ? 1 : 0;
}

int CloudManager::cloudScanI2C(String command) {
    Log.info("CLOUD: Scanning I2C Bus...");
    
    int devicesFound = 0;
    int firstAddress = -1;

    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            Log.info("I2C Device found at address: 0x%02X", address);
            if (firstAddress == -1) firstAddress = address;
            devicesFound++;
        }
    }
    
    return (devicesFound > 0) ? firstAddress : -1;
}

// RPC: Get Full System Status as JSON
int CloudManager::cloudGetStatus(String command) {
    Log.info("CLOUD: Generating Status JSON...");

    // Create JSON Document
    JsonDocument doc;

    // Add Hardware Data
    doc["counters"]["a"] = core_get_counter_a();
    doc["counters"]["b"] = core_get_counter_b();
    doc["counters"]["c"] = core_get_counter_c();

    // Add System Info
    doc["system"]["uptime"] = System.uptime();
    doc["system"]["free_mem"] = System.freeMemory();
    doc["system"]["device_id"] = System.deviceID(); 
    doc["system"]["wifi_strength"] = WiFi.RSSI().getStrength();

    // Serialize to a char buffer
    char outputBuffer[1024];
    
    // Clear buffer to be safe
    memset(outputBuffer, 0, sizeof(outputBuffer));

    // Serialize
    size_t bytesWritten = serializeJson(doc, outputBuffer, sizeof(outputBuffer));

    // Convert to Particle String for publishing
    String jsonOutput = String(outputBuffer);

    // Publish Event
    Particle.publish("app/status/report", jsonOutput, PRIVATE);
    serializeJson(doc, _jsonBuffer, sizeof(_jsonBuffer));

    Log.info("Status sent (%d bytes): %s", bytesWritten, outputBuffer);

    return 1;
}

// --- Event Handler ---
void CloudManager::handleCloudEvent(const char *event, const char *data) {
    Log.info("CLOUD: Evento recibido: %s", event);
    cloudGetStatus(""); 
}