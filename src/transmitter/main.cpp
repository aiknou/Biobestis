#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN  D10
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println("Transmitter ready.");
}

void loop() {
    float humidity    = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Sensor read failed — check wiring and DHT_TYPE");
    } else {
        Serial.printf("Temp: %.1f C  Humidity: %.1f %%\n", temperature, humidity);
    }

    delay(20);
}
