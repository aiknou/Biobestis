#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "eye_display.h"
#include "tca9548a.h"
#include "art/bitmaps.h"

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   32
#define OLED_RESET      -1
#define SCREEN_ADDRESS 0x3C
#define TCA_CHANNEL     0

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

EyeDisplayController eyeCtrl;

void EyeDisplayController::init() {
    tcaSelect(TCA_CHANNEL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("eyeDisplay init failed");
        return;
    }
    display.clearDisplay();
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void EyeDisplayController::test() {
    tcaSelect(TCA_CHANNEL);
    display.fillScreen(SSD1306_WHITE);
    display.display();
    delay(500);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("eyeDisplay OK");
    tcaSelect(TCA_CHANNEL);
    display.display();
    delay(2000);

    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void EyeDisplayController::drawBitmap(const unsigned char* bitmap) {
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, EYE_W, EYE_H, SSD1306_WHITE);
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void EyeDisplayController::clear() {
    display.clearDisplay();
    tcaSelect(TCA_CHANNEL);
    display.display();
}
