/*
 * File: src/system/rtos_manager.h
 * Description: Definitions for RTOS objects and Data Structures
 */

#ifndef RTOS_MANAGER_H
#define RTOS_MANAGER_H

#include "Particle.h"

// --- Config ---
#define SENSOR_QUEUE_SIZE 10

// --- Message ---
struct SensorMessage {
    uint32_t timestamp;
    float filteredValue;
    
    int countA;
    int countB;
    int countC;
};

// --- Shared State ---
struct SystemSharedState {
    volatile float latestSensorValue;
    volatile int totalTriggers;

    volatile int countA; 
    volatile int countB; 
    volatile int countC; 
};

// --- Global Objects ---
extern os_queue_t sensorQueue;
extern os_mutex_t i2cMutex;
extern SystemSharedState sharedState;

#endif // RTOS_MANAGER_H