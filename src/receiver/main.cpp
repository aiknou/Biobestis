#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <wifi_controller.h>
#include "eye_display.h"
#include "data_display.h"
#include "art/bitmaps.h"

const int led = D10;
const int resetBtn = D1;

// RTC on D6=CLK, D7=DAT, D8=RST
ThreeWire rtcWire(D7, D6, D8);
RtcDS1302<ThreeWire> rtc(rtcWire);

int fullnessamount = 0;
int humidity = 65;
unsigned long lastDataSwitch = 0;
unsigned long lastDayCheck = 0;
int dataState = 0;  // 0=fullness, 1=humidity, 2=emptied days

static const char* statusLabels[6] = {
    "Great!", "Good", "OK", "Meh", "Bad", "Critical!"
};

// Store/load the "last emptied" unix timestamp in DS1302 RAM (4 bytes at addr 0-3)
void saveEmptyTimestamp(uint32_t ts) {
    rtc.SetMemory((uint8_t)0, (uint8_t)((ts >> 24) & 0xFF));
    rtc.SetMemory((uint8_t)1, (uint8_t)((ts >> 16) & 0xFF));
    rtc.SetMemory((uint8_t)2, (uint8_t)((ts >>  8) & 0xFF));
    rtc.SetMemory((uint8_t)3, (uint8_t)( ts        & 0xFF));
}

uint32_t loadEmptyTimestamp() {
    uint32_t ts = 0;
    ts |= ((uint32_t)rtc.GetMemory((uint8_t)0)) << 24;
    ts |= ((uint32_t)rtc.GetMemory((uint8_t)1)) << 16;
    ts |= ((uint32_t)rtc.GetMemory((uint8_t)2)) <<  8;
    ts |=  (uint32_t)rtc.GetMemory((uint8_t)3);
    return ts;
}

int getDaysSinceEmpty() {
    uint32_t emptyTs = loadEmptyTimestamp();
    if (emptyTs == 0) return 0;
    uint32_t nowTs = rtc.GetDateTime().Unix32Time();
    if (nowTs < emptyTs) return 0;
    return (nowTs - emptyTs) / 86400;
}

void updateEyeStatus() {
    int days = getDaysSinceEmpty();
    int worstSegment = max({min(fullnessamount / 17, 5), min(humidity / 17, 5), min(days, 5)});
    eyeCtrl.drawBitmap((const unsigned char*)pgm_read_ptr(&statusBitmaps[worstSegment]));
}

void drawCurrentDataScreen() {
    if (dataState == 0) {
        dataCtrl.drawTextWithProgress("Fullness state", fullnessamount);
    } else if (dataState == 1) {
        char buf[20];
        snprintf(buf, sizeof(buf), "Humidity: %d %%", humidity);
        dataCtrl.drawCenteredText(buf, 1);
    } else {
        char buf[24];
        snprintf(buf, sizeof(buf), "Emptied %d days ago", getDaysSinceEmpty());
        dataCtrl.drawCenteredText(buf, 1);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(led, OUTPUT);
    pinMode(resetBtn, INPUT_PULLUP);
    wifi_connect("aalto open", "");

    rtc.Begin();
    if (!rtc.IsDateTimeValid() || !rtc.GetIsRunning()) {
        Serial.println("RTC not set — using compile time. Use 'settime' to update.");
        rtc.SetIsRunning(true);
        rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }

    Wire.begin(D4, D5);
    eyeCtrl.init();
    updateEyeStatus();

    dataCtrl.init();
    dataCtrl.test();
    drawCurrentDataScreen();
}

void handleSerial() {
    if (!Serial.available()) return;
    String line = Serial.readStringUntil('\n');
    line.trim();

    // settime is case-sensitive input — handle before toLowerCase
    if (line.startsWith("settime ")) {
        // format: settime YYYY-MM-DD HH:MM:SS
        String dt = line.substring(8);
        int year  = dt.substring(0, 4).toInt();
        int month = dt.substring(5, 7).toInt();
        int day   = dt.substring(8, 10).toInt();
        int hour  = dt.substring(11, 13).toInt();
        int min   = dt.substring(14, 16).toInt();
        int sec   = dt.substring(17, 19).toInt();
        RtcDateTime newTime(year - 2000, month, day, hour, min, sec);
        rtc.SetDateTime(newTime);
        Serial.println("RTC time set.");
        return;
    }

    line.toLowerCase();

    if (line.startsWith("humidity ")) {
        humidity = line.substring(9).toInt();
        Serial.print("Humidity set to ");
        Serial.println(humidity);
        if (dataState == 1) drawCurrentDataScreen();
        updateEyeStatus();
    } else if (line.startsWith("fullness ")) {
        fullnessamount = constrain(line.substring(9).toInt(), 0, 100);
        Serial.print("Fullness set to ");
        Serial.println(fullnessamount);
        if (dataState == 0) drawCurrentDataScreen();
        updateEyeStatus();
    } else if (line == "day" || line.startsWith("day ")) {
        // Simulate days by shifting the saved empty timestamp backwards
        int days = (line == "day") ? getDaysSinceEmpty() + 1 : line.substring(4).toInt();
        uint32_t nowTs = rtc.GetDateTime().Unix32Time();
        saveEmptyTimestamp(nowTs - (uint32_t)days * 86400);
        Serial.print("Days since empty set to ");
        Serial.println(days);
        if (dataState == 2) drawCurrentDataScreen();
        updateEyeStatus();
    } else if (line == "time") {
        RtcDateTime now = rtc.GetDateTime();
        Serial.printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            now.Year(), now.Month(), now.Day(),
            now.Hour(), now.Minute(), now.Second());
    } else if (line == "status") {
        RtcDateTime now = rtc.GetDateTime();
        Serial.printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            now.Year(), now.Month(), now.Day(),
            now.Hour(), now.Minute(), now.Second());
        Serial.printf("Days since empty: %d\n", getDaysSinceEmpty());
        Serial.printf("Fullness: %d  Humidity: %d\n", fullnessamount, humidity);
    }
}

void loop() {
    handleSerial();

    static bool btnWasPressed = false;
    bool btnPressed = digitalRead(resetBtn) == LOW;
    if (btnPressed && !btnWasPressed) {
        Serial.println("Button pressed! Resetting fullness and empty timestamp.");
        fullnessamount = 0;
        saveEmptyTimestamp(rtc.GetDateTime().Unix32Time());
        drawCurrentDataScreen();
        updateEyeStatus();
    }
    btnWasPressed = btnPressed;

    // Refresh days display once per minute
    if (millis() - lastDayCheck >= 60000) {
        lastDayCheck = millis();
        if (dataState == 2) drawCurrentDataScreen();
        updateEyeStatus();
    }

    if (millis() - lastDataSwitch >= 10000) {
        lastDataSwitch = millis();
        dataState = (dataState + 1) % 3;
        drawCurrentDataScreen();
    }
}
