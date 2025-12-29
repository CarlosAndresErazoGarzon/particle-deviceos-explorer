/*
 * Project particle-deviceos-explorer
 * Branch: feature/07-cloud-primitives
 * Author: Carlos Erazo
 */

#include "Particle.h"
#include "system/rtos_manager.h"

// Drivers
#include "drivers/gpio_core.h"
#include "drivers/storage_core.h"
#include "drivers/timer_logic.h"

// Modules
#include "modules/cloud_task.h"
#include "modules/ui_task.h"

// Global RTOS Objects
os_queue_t sensorQueue;
os_mutex_t i2cMutex;

// Threads 
os_thread_t sensorThreadHandle;
os_thread_t uiThreadHandle;

// Tasks
void ui_init();
void sensorTaskFunction(void *param);
void systemEventHandler(system_event_t event, int param);


// Global Objects
CloudManager cloud;
volatile bool systemOnline = false;

SerialLogHandler logHandler(LOG_LEVEL_INFO);

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// Hardware Definitions
const int BTN_A = D4;
const int BTN_B = D3;
const int BTN_C = D2;
const int LED_USER = D7;

void setup()
{
    // Debug Only: Uncomment to halt boot until Serial connects
    // waitFor(Serial.isConnected, 10000);
    // delay(1000);

    // Log.info("=== RTOS SYSTEM BOOT ===");

    // RTOS Primitives
    os_queue_create(&sensorQueue, sizeof(SensorMessage), SENSOR_QUEUE_SIZE, 0);
    os_mutex_create(&i2cMutex);

    // Init Drivers
    core_init(millis);

    // Pin config
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_C, INPUT_PULLUP);
    pinMode(LED_USER, OUTPUT);

    // Interruptions
    attachInterrupt(BTN_A, core_isr_btn_a, FALLING);
    attachInterrupt(BTN_B, core_isr_btn_b, FALLING);
    attachInterrupt(BTN_C, core_isr_btn_c, FALLING);

    // Init UI & Cloud
    ui_init();
    cloud.begin();
    System.on(cloud_status, systemEventHandler);

    // Log.info("RTOS: Spawning Threads...");

    // CREATE THREADS
    // Sensor Thread
    os_thread_create(
        &sensorThreadHandle, 
        "SensorTask", 
        OS_THREAD_PRIORITY_DEFAULT, 
        sensorTaskFunction, 
        NULL, 
        6000
    );

    // UI Thread
    os_thread_create(
        &uiThreadHandle, 
        "UITask", 
        OS_THREAD_PRIORITY_DEFAULT,
        uiTaskFunction, 
        NULL, 
        6000
    );

    // Log.info("RTOS: Setup Complete.");
}

// --- MAIN LOOP ---
void loop()
{
    // UI Status Handler
    static bool lastOnline = false;
    if (systemOnline != lastOnline)
    {
        ui_set_online_status(systemOnline);
        lastOnline = systemOnline;
    }

    // RTOS -> Cloud Bridge
    static unsigned long lastSync = 0;
    if (millis() - lastSync > 2000) {
        cloud.sync(
            sharedState.countA, 
            sharedState.countB, 
            sharedState.countC,
            (double) sharedState.latestSensorValue
        );
        lastSync = millis();
    }
}

// --- EVENT HANDLER ---
void systemEventHandler(system_event_t event, int param) {
    if (event == cloud_status) {
        systemOnline = (param == cloud_status_connected);
    }
}
