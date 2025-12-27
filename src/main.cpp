/* 
 * Project particle-deviceos-explorer
 * Author: Carlos Erazo
 * Date: 
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

const int BTN_A = D4; 
const int BTN_B = D3;
const int BTN_C = D2;


Adafruit_SSD1306 display(128, 32, &Wire, -1);


SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void printUI(String title, String subtitle) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(title);
    display.setCursor(0,16);
    display.println(subtitle);
    display.display();
}

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_C, INPUT_PULLUP);
    

    printUI("SISTEMA LISTO", "Esperando...");
}


void loop() {

    if (digitalRead(BTN_A) == LOW) {
        printUI("BOTON A", "Presionado");
        delay(100); 
    }
    
    if (digitalRead(BTN_B) == LOW) {
        printUI("BOTON B", "Presionado");
        delay(100);
    }

    if (digitalRead(BTN_C) == LOW) {
        printUI("BOTON C", "Presionado");
        delay(100);
    }
}
