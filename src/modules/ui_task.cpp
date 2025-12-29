/*
 * File: src/modules/ui_task.cpp
 * Description: Implementation of UIManager methods.
 */

#include "ui_task.h"
#include "../system/rtos_manager.h"

// Global Variables
static Adafruit_SSD1306 display(128, 32, &Wire, -1);
static bool _isOnline = false;

// Hardware Init
void ui_init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.clearDisplay();
    display.display();
    // Log.info("RTOS: UI Hardware Initialized");
}

// Internal Render Function
void _renderInternal(SensorMessage *data)
{
    display.clearDisplay();

    // Header
    display.setCursor(0, 0);
    display.print("RTOS SYSTEM");

    // Online Indicator
    if (_isOnline)
        display.fillCircle(120, 4, 3, WHITE);
    else
        display.drawCircle(120, 4, 3, WHITE);

    // Row 1: Counters
    display.setCursor(0, 12);
    display.printf("A:%d B:%d C:%d", data->countA, data->countB, data->countC);

    // Row 2: Sensor Value
    display.setCursor(0, 22);
    display.printf("Sensor: %.1f", data->filteredValue);

    // Visual Progress Bar
    int barWidth = (int)(data->filteredValue * 128 / 100);
    display.fillRect(0, 30, barWidth, 2, WHITE);

    display.display();
}

// --- RTOS TASK ---
void uiTaskFunction(void *param) {
    // Log.info("RTOS: [UI] Thread Running. Waiting for data...");

    SensorMessage msg;

    while(true) {
        // Wait for data with 1000ms
        int result = os_queue_take(sensorQueue, &msg, 1000, NULL);
        
        if (result == 0) {       
            // Data received 
            os_mutex_lock(i2cMutex);
            _renderInternal(&msg);
            os_mutex_unlock(i2cMutex);
        }
    }
}

// --- Helper ---
void ui_set_online_status(bool status)
{
    _isOnline = status;
}