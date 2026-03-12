#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "data_display.h"
#include "tca9548a.h"

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   32
#define OLED_RESET      -1
#define SCREEN_ADDRESS 0x3C
#define TCA_CHANNEL     1

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DataDisplayController dataCtrl;

void DataDisplayController::init() {
    tcaSelect(TCA_CHANNEL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("dataDisplay init failed");
        return;
    }
    display.clearDisplay();
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void DataDisplayController::test() {
    tcaSelect(TCA_CHANNEL);
    display.fillScreen(SSD1306_WHITE);
    display.display();
    delay(500);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("dataDisplay OK");
    tcaSelect(TCA_CHANNEL);
    display.display();
    delay(2000);

    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void DataDisplayController::drawCenteredText(const char* text, int textSize) {
    int16_t x1, y1;
    uint16_t w, h;
    display.clearDisplay();
    display.setTextSize(textSize);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
    display.setTextColor(SSD1306_WHITE);
    display.print(text);
    tcaSelect(TCA_CHANNEL);
    display.display();
}

void DataDisplayController::drawTextWithProgress(const char* text, int progress) {
    // Layout (vertically centered): text (8px) + 4px gap + bar (6px) = 18px total
    const int startY   = (SCREEN_HEIGHT - 18) / 2; // = 7
    const int barY     = startY + 12;
    const int barH     = 6;
    const int barX     = 2;
    const int barW     = SCREEN_WIDTH - 4;
    const int fillW    = (barW * constrain(progress, 0, 100)) / 100;

    int16_t x1, y1;
    uint16_t w, h;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, startY);
    display.print(text);

    display.drawRect(barX, barY, barW, barH, SSD1306_WHITE);
    if (fillW > 0)
        display.fillRect(barX, barY, fillW, barH, SSD1306_WHITE);

    tcaSelect(TCA_CHANNEL);
    display.display();
}

void DataDisplayController::clear() {
    display.clearDisplay();
    tcaSelect(TCA_CHANNEL);
    display.display();
}
