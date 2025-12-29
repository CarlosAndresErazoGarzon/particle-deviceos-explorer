/*
 * File: src/cpp_modules/UIManager.h
 * Description: Class handling OLED display logic.
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

class UIManager {
public:
    // Constructor
    UIManager();

    // Initialize hardware (I2C)
    bool begin();

    // Render the Dashboard view
    void renderDashboard(int countA, int countB, int countC, bool isOnline);

    // Show status messages
    void showMessage(const char* title, const char* subtitle);

private:
    Adafruit_SSD1306 display;
};

#endif // UI_MANAGER_H