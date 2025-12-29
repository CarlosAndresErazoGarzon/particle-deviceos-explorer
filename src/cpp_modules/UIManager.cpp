/*
 * File: src/cpp_modules/UIManager.cpp
 * Description: Implementation of UIManager methods.
 */

#include "UIManager.h"

// Constructor: Initialize the Adafruit object with default I2C address
UIManager::UIManager() : display(128, 32, &Wire, -1) {
}

bool UIManager::begin() {
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

void UIManager::renderDashboard(int countA, int countB, int countC, bool isOnline) {
display.clearDisplay();
    
    // --- Header ---
    display.setCursor(0,0);
    display.print("DEV OS EXPLORER");

    if (isOnline) {
        display.fillCircle(120, 4, 3, WHITE);
    } else {
        display.drawCircle(120, 4, 3, WHITE);
        display.drawLine(117, 1, 123, 7, WHITE);
    }

    display.setCursor(0, 12);
    display.setTextSize(1);
    display.printf("A: %d", countA);
    
    display.setCursor(64, 12); 
    display.printf("B: %d", countB);
    
    display.setCursor(0, 22);
    display.printf("C: %d", countC);

    display.display();
}