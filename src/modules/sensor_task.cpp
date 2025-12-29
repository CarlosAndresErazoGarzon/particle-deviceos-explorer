/*
 * File: src/modules/sensor_task.cpp
 * Description: High Priority Thread for Sensor Acquisition & Logic
 */

#include "Particle.h"
#include "../system/rtos_manager.h"
#include "../algorithms/ema_filter.h"
#include "../drivers/gpio_core.h"

static EmaFilter myFilter;
SystemSharedState sharedState;

// Signal generator
float simulateSensor() {
    static float angle = 0.0;
    float val = sin(angle) * 50.0 + 50.0; 
    angle += 0.1;
    if (angle > 6.28) angle = 0;
    return val + ((rand() % 10) - 5);
}

// Worker Thread
void sensorTaskFunction(void *param) {
    ema_init(&myFilter, 0.1f);
    const int TASK_PERIOD_MS = 50; 
    system_tick_t lastWakeTime = millis();

    int decimationCounter = 0;
    
    // Memory to detect button changes
    int lastA = -1;
    int lastB = -1;
    int lastC = -1;

    // Memory to freeze the sensor value on the UI
    float lastVisualSensorValue = 0.0;

    // Log.info("RTOS: [SENSOR] Thread Started");

    while(true) {
        // Adquisición
        float raw = simulateSensor();
        float filtered = ema_apply(&myFilter, raw);
        
        int a = core_get_counter_a();
        int b = core_get_counter_b();
        int c = core_get_counter_c();

        // Global State Update
        sharedState.latestSensorValue = filtered;
        sharedState.countA = a;
        sharedState.countB = b;
        sharedState.countC = c;
        sharedState.totalTriggers = a + b + c;

        // Decoupled UI Logic
        bool buttonsChanged = (a != lastA || b != lastB || c != lastC);
        decimationCounter++;
        
        // Timer
        bool timeToUpdateSensor = (decimationCounter >= 10000);

        if (timeToUpdateSensor || buttonsChanged) { 
            SensorMessage msg;
            msg.timestamp = millis();
            
            if (timeToUpdateSensor) {
                // Time-based update
                msg.filteredValue = filtered;
                lastVisualSensorValue = filtered;
                decimationCounter = 0; 
                // Log.info("RTOS: Periodic Update (New Sensor Value)");
            } else {
                // Button-based update
                msg.filteredValue = lastVisualSensorValue;
                // Log.info("RTOS: Button Update (Frozen Sensor Value)");
            }
            // ---------------------------

            msg.countA = a; 
            msg.countB = b; 
            msg.countC = c;
            
            // Send to UI Queue
            os_queue_put(sensorQueue, &msg, 0, 0);

            // Update button memory
            lastA = a;
            lastB = b;
            lastC = c;
        }

        os_thread_delay_until(&lastWakeTime, TASK_PERIOD_MS);
    }
}