#include <wifi_controller.h>

void wifi_connect(const char* ssid, const char* password) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
}

bool wifi_is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

