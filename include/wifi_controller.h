#pragma once
#include <WiFi.h>

void wifi_connect(const char* ssid, const char* password);
bool wifi_is_connected();
