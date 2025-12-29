/*
 * File: src/modules/ui_task.h
 * Description: Class handling OLED display logic.
 */

#ifndef UI_TASK_H
#define UI_TASK_H

#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

// Main Task Prototype 
void uiTaskFunction(void *param);

// Helper Prototype for status updates
void ui_set_online_status(bool status);

#endif // UI_TASK_H