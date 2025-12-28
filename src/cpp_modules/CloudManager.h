/*
 * File: src/cpp_libraries/CloudManager.h
 * Description: Interface for Particle Cloud primitives.
 */

#ifndef CLOUD_MANAGER_H
#define CLOUD_MANAGER_H

#include "Particle.h"

class CloudManager {
public:
    CloudManager();
    
    // Variables & functions
    void begin();

    // Updates
    void sync(int a, int b, int c);

private:
    // Shadow Variables
    int _cloudCountA;
    int _cloudCountB;
    int _cloudCountC;

    static char _jsonBuffer[622];

    // Cloud Functions Callbacks
    static int cloudResetCounters(String command);
    static int cloudToggleLed(String command);
    static int cloudSaveData(String command);
    static int cloudDeleteData(String command);
    static int cloudCheckData(String command);
    static int cloudScanI2C(String command);
    static int cloudGetStatus(String command);

    // Event Handler
    void handleCloudEvent(const char *event, const char *data);
};

#endif // CLOUD_MANAGER_H