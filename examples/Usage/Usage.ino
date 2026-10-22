#include <Bonezegei_TM1638.h>

#define STB_PIN 4
#define CLK_PIN 3
#define DIO_PIN 2

Bonezegei_TM1638 tm(STB_PIN, CLK_PIN, DIO_PIN);

void setup() {
  Serial.begin(115200);
  tm.begin();
  tm.setBrightness(5); // Brightness 0..7
  
  // Display text
  tm.displayString("HELLO88");
  delay(1500);
  
  tm.clear();
}

void loop() {
  // Read button states (S1 - S8)
  uint8_t buttons = tm.getButtons();
  
  // Light up corresponding LEDs matching button presses
  tm.setLEDs(buttons);
  
  // Display button bitmask state as number
  tm.displayNumber(buttons);
  
  delay(50);
}