/*
 * File: src/cpp_libraries/UIManager.cpp
 * Description: Implementation of UIManager methods.
 */

#include "UIManager.h"

// Constructor: Initialize the Adafruit object with default I2C address
UIManager::UIManager() : display(128, 32, &Wire, -1) {
}

bool UIManager::begin() {
    // SSD1306_SWITCHCAPVCC generates display voltage from 3.3V internal
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return false;
    }
    
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.clearDisplay();
    display.display();
    return true;
}

void UIManager::showMessage(const char* title, const char* subtitle) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(title);
    if (subtitle) {
        display.setCursor(0,16);
        display.println(subtitle);
    }
    display.display();
}

void UIManager::renderDashboard(int countA, int countB, int countC, bool dataSaved) {
    display.clearDisplay();
    
    // Header
    display.setCursor(0,0);
    if (dataSaved) {
        display.println("SAVED [OK]");
    } else {
        display.println("System Running...");
    }

    // Counters Row
    display.setCursor(0, 12);
    display.printf("A:%d  B:%d", countA, countB);
    
    display.setCursor(0, 22);
    display.printf("C:%d", countC);
    
    display.display();
}