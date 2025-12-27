/* 
 * Project particle-deviceos-explorer
 * Branch: feature/01-interrupts-gpio
 * Author: Carlos Erazo
 * Description: GPIO management with software interruptions
 */


#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "c_modules/gpio_core.h"
#include "c_modules/timer_logic.h"

// Wrapper
extern "C" uint32_t bridge_millis() {
    return (uint32_t)millis();
}

// --------------------------------------------------------
// DEFINITIONS & HARDWARE MAPPING
// --------------------------------------------------------

// Buttons
const int BTN_A = D4; 
const int BTN_B = D3;
const int BTN_C = D2;
const int LED_HEARTBEAT = D7;

// Display
Adafruit_SSD1306 display(128, 32, &Wire, -1);

// --------------------------------------------------------
// SOFTWARE TIMERS CALLBACKS
// --------------------------------------------------------

void onHeartbeatTick() {

    bool newState = timer_core_toggle_heartbeat();
    digitalWrite(LED_HEARTBEAT, newState);
}
Timer heartbeatTimer(500, onHeartbeatTick);

// --------------------------------------------------------
// SYS CONFIG
// --------------------------------------------------------

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

// --------------------------------------------------------
// HARDWARE CONFIG
// --------------------------------------------------------

LEDStatus customTheme(LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_IMPORTANT);

void hardwareConfig() {
    customTheme.setColor(0x00030303); 
    customTheme.setActive(true);
}
STARTUP(hardwareConfig());

// --------------------------------------------------------
// UI LOGIC
// --------------------------------------------------------

void renderUI() {
    display.clearDisplay();

    display.setCursor(0,0);
    display.setTextSize(1);
    display.println("Timers Running...");

    display.setCursor(0, 12);
    display.printf("A:%d  B:%d", core_get_counter_a(), core_get_counter_b());
    display.setCursor(0, 22);
    display.printf("C:%d", core_get_counter_c());
    
    display.display();
}

// --------------------------------------------------------
// SETUP
// --------------------------------------------------------

void setup() {
    // Dependency Injection
    core_init(bridge_millis);

    // I2C/Screen initialization
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        display.setTextColor(WHITE);
        display.setTextSize(1);
        renderUI();
    }

    // Pin config
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_C, INPUT_PULLUP);
    pinMode(LED_HEARTBEAT, OUTPUT);

    // Interruptions
    attachInterrupt(BTN_A, core_isr_btn_a, FALLING);
    attachInterrupt(BTN_B, core_isr_btn_b, FALLING);
    attachInterrupt(BTN_C, core_isr_btn_c, FALLING);

    // Timers
    heartbeatTimer.start();
}


void loop() {
    // Check if state changed in the Core
    noInterrupts();
    bool updateNeeded = core_check_and_clear_ui_flag();
    interrupts();

    if (updateNeeded) {
        renderUI();
    }
}
