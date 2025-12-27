/* 
 * Project particle-deviceos-explorer
 * Branch: feature/03-filesystem-littlefs
 * Author: Carlos Erazo
 */


#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "c_modules/gpio_core.h"
#include "c_modules/timer_logic.h"
#include "c_modules/storage_core.h"

#include "cpp_modules/UIManager.h"

// Magic Key for validation
#define MAGIC_VALIDATION_KEY 0xCAFEBABE
#define SAVE_DELAY_MS 5000

// --------------------------------------------------------
// DEFINITIONS & HARDWARE MAPPING
// --------------------------------------------------------

// Buttons
const int BTN_A = D4; 
const int BTN_B = D3;
const int BTN_C = D2;
const int LED_HEARTBEAT = D7;

void onHeartbeatTick();
void performSafeSave();
void loadData();
void processAppState();

// Global Objects
UIManager ui;
Timer heartbeatTimer(500, onHeartbeatTick);

// State Variables
bool pendingSave = false;
unsigned long lastChangeTime = 0;
bool lastSaveStatus = false;

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

// --- Callbacks & Helpers ---
extern "C" uint32_t bridge_millis() {
    return (uint32_t)millis();
}

void onHeartbeatTick() {

    bool newState = timer_core_toggle_heartbeat();
    digitalWrite(LED_HEARTBEAT, newState);
}

// --- Application Logic Functions ---

void performSafeSave() {
    AppState currentState;
    currentState.countA = core_get_counter_a();
    currentState.countB = core_get_counter_b();
    currentState.countC = core_get_counter_c();
    currentState.magic_number = MAGIC_VALIDATION_KEY;
    
    storage_save_state(&currentState);
}

void loadData() {
    AppState savedState;
    if (storage_load_state(&savedState)) {
        core_set_counters(savedState.countA, savedState.countB, savedState.countC);
        ui.showMessage("DATA RESTORED", "Welcome Back");
    } else {
        ui.showMessage("NEW SESSION", "No Data Found");
    }
    delay(1000);
}

void processAppState() {
    // Static variables preserve state between function calls
    static bool pendingSave = false;
    static unsigned long lastChangeTime = 0;

    // Check for Hardware Events
    noInterrupts();
    bool hardwareStateChanged = core_check_and_clear_ui_flag();
    interrupts();

    // 2. React to Changes
    if (hardwareStateChanged) {
        // Immediate UI feedback
        ui.renderDashboard(
            core_get_counter_a(), 
            core_get_counter_b(), 
            core_get_counter_c(), 
            false // Not saved yet
        );
        
        // Schedule deferred save
        pendingSave = true;
        lastChangeTime = millis();
    }

    // Handle Deferred Save (Time-triggered)
    if (pendingSave && (millis() - lastChangeTime > SAVE_DELAY_MS)) {
        performSafeSave();
        
        // Reset Logic
        pendingSave = false;
        
        // Update UI to show confirmation
        ui.renderDashboard(
            core_get_counter_a(), 
            core_get_counter_b(), 
            core_get_counter_c(), 
            true // Saved 
        );
    }
}

// --------------------------------------------------------
// SETUP
// --------------------------------------------------------

void setup() {
    // Dependency Injection
    core_init(bridge_millis);

    // I2C/Screen initialization
    ui.begin();

    // Initialize Data
    loadData();

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

    // Initial Render
    ui.renderDashboard(core_get_counter_a(), core_get_counter_b(), core_get_counter_c(), false);
}


void loop() {
    processAppState();
}
