/* 
 * Project particle-deviceos-explorer
 * Author: Carlos Erazo
 * Date: 
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

Adafruit_SSD1306 display(128, 32, &Wire, -1);


SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void hardwareConfig() {
    if_wiznet_pin_remap remap = {};
    remap.base.type = IF_WIZNET_DRIVER_SPECIFIC_PIN_REMAP;
    remap.cs_pin = D5;             
    remap.reset_pin = PIN_INVALID; 
    remap.int_pin   = PIN_INVALID;
    
    if_request(nullptr, IF_REQ_DRIVER_SPECIFIC, &remap, sizeof(remap), nullptr);
    System.enableFeature(FEATURE_ETHERNET_DETECTION);
}
STARTUP(hardwareConfig());

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("SISTEMA ONLINE");
    display.println("Modo: Automatico");
    display.display();
}


void loop() {

}
