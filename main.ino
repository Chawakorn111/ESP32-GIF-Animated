#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "NIG.h"
#include "rickroll.h"
#include "waguri.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define TOUCH_PIN 3  

int currentGIF = 0;
bool changeGIF = false;
int lastButtonState = HIGH;
float speedFactor = 1.0;  // ✅ Added speed factor

// -----------------------------
// ฟังก์ชันเล่น GIF
// -----------------------------
void playGIF(const AnimatedGIF* gif, uint16_t loopCount = 1) {
  for (uint16_t loop = 0; loop < loopCount; loop++) {
    for (uint16_t frame = 0; frame < gif->frame_count; frame++) {

        int buttonState = digitalRead(TOUCH_PIN);
        if (lastButtonState == HIGH && buttonState == LOW) {
            currentGIF = (currentGIF + 1) % 3; // มี 3 GIF
            changeGIF = true;
            lastButtonState = buttonState;
            display.clearDisplay();
            display.display();
            delay(50);
            return;
        }
        lastButtonState = buttonState;

        display.clearDisplay();

        // วาด frame ปัจจุบัน (อ่านจาก PROGMEM)
        for (uint16_t y = 0; y < gif->height; y++) {
            for (uint16_t x = 0; x < gif->width; x++) {
                uint16_t byteIndex = y * (((gif->width + 7) / 8)) + (x / 8);
                uint8_t pixelByte = pgm_read_byte(&(gif->frames[frame][byteIndex]));
                uint8_t bitIndex = 7 - (x % 8);
                if (pixelByte & (1 << bitIndex)) {
                    display.drawPixel(x, y, WHITE);
                }
            }
        }

        display.display();
        
        uint16_t delayTime = (uint16_t)(pgm_read_word(&(gif->delays[frame])) * speedFactor);
        delay(delayTime);
    }
  }
}

// -----------------------------
// ฟังก์ชันเลือก GIF
// -----------------------------
void display_image() {
  if (currentGIF == 0) {
      
      playGIF(&rickroll_gif);
  } else if (currentGIF == 1) {
      playGIF(&nig_gif);
  } else if (currentGIF == 2) {
      speedFactor = 0.35;
      playGIF(&waguri_gif);
      speedFactor = 1;            
  }
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
    Wire.begin(20, 21);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 init failed!");
        while(1);
    }
    
    pinMode(TOUCH_PIN, INPUT_PULLUP);
    display.clearDisplay();
    display.display();
}

// -----------------------------
// Loop
// -----------------------------
void loop() {
  changeGIF = false;
  display_image();
}
