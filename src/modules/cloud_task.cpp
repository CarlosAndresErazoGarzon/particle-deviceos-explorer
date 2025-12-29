/*
 * File: src/cpp_modules/CloudManager.cpp
 * Description: Implementation of Cloud primitives.
 */

#include "cloud_task.h"
#include "../system/rtos_manager.h"

#include "../drivers/gpio_core.h"
#include "../drivers/storage_core.h"
#include "../drivers/timer_logic.h"

#include <ArduinoJson.h>


static bool _ledState = false;
const int JSON_CAPACITY = 1024;
char CloudManager::_jsonBuffer[622];


CloudManager::CloudManager() {
    _cloudCountA = 0;
    _cloudCountB = 0;
    _cloudCountC = 0;
    _cloudSensorValue = 0.0;
}

void CloudManager::begin() {
    // Variables
    Particle.variable("cnt_a", _cloudCountA);
    Particle.variable("cnt_b", _cloudCountB);
    Particle.variable("cnt_c", _cloudCountC);
    Particle.variable("sensor_value", (double)_cloudSensorValue);
    Particle.variable("json_status", _jsonBuffer);

    // Functions
    Particle.function("reset_all", cloudResetCounters);
    Particle.function("toggle_led", cloudToggleLed);
    Particle.function("save_data", cloudSaveData);
    Particle.function("del_data", cloudDeleteData);
    Particle.function("chk_data", cloudCheckData);
    Particle.function("scan_i2c", cloudScanI2C);
    Particle.function("get_status", cloudGetStatus);

    // Event Subscription
    Particle.subscribe("cmd/status", &CloudManager::handleCloudEvent, this);
}

void CloudManager::sync(int a, int b, int c, float sensorValue) {
    _cloudCountA = a;
    _cloudCountB = b;
    _cloudCountC = c;
    _cloudSensorValue = sensorValue;

    updateJsonBuffer();
}

// --- RPC Callbacks ---

int CloudManager::cloudToggleLed(String command) {
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

int CloudManager::cloudResetCounters(String command) {
    core_set_counters(0, 0, 0);
    storage_reset_data();
    
    return 1;
}

int CloudManager::cloudSaveData(String command) {
    AppState currentState;
    currentState.countA = core_get_counter_a();
    currentState.countB = core_get_counter_b();
    currentState.countC = core_get_counter_c();
    currentState.magic_number = 0xCAFEBABE;

    bool success = storage_save_state(&currentState);
    
    return success ? 1 : -1;
}

int CloudManager::cloudDeleteData(String command) {
    return storage_reset_data() ? 1 : 0;
}

int CloudManager::cloudCheckData(String command) {
    return storage_file_exists() ? 1 : 0;
}

int CloudManager::cloudScanI2C(String command) {
    // Log.info("CLOUD: Scanning I2C Bus...");
    
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

int CloudManager::cloudGetStatus(String command) {
    // Log.info("CLOUD: Generating RTOS JSON...");

    JsonDocument doc;

    doc["counters"]["a"] = core_get_counter_a();
    doc["counters"]["b"] = core_get_counter_b();
    doc["counters"]["c"] = core_get_counter_c();
    doc["sensor"]["value"] = sharedState.latestSensorValue;
    doc["sensor"]["total_triggers"] = sharedState.totalTriggers;
    doc["system"]["uptime"] = System.uptime();
    doc["system"]["free_mem"] = System.freeMemory();
    
    char outputBuffer[512];
    serializeJson(doc, outputBuffer);

    Particle.publish("app/status/rtos", outputBuffer, PRIVATE);

    return 1;
}

void CloudManager::handleCloudEvent(const char *event, const char *data) {
    Log.info("CLOUD: Evento recibido: %s", event);
    cloudGetStatus(""); 
}

void CloudManager::updateJsonBuffer() {
    JsonDocument doc;

    doc["cnt"]["a"] = _cloudCountA;
    doc["cnt"]["b"] = _cloudCountB;
    doc["cnt"]["c"] = _cloudCountC;
    doc["sensor"] = _cloudSensorValue;
    doc["sys"]["upt"] = System.uptime();
    doc["sys"]["mem"] = System.freeMemory();
    serializeJson(doc, _jsonBuffer, sizeof(_jsonBuffer));
}