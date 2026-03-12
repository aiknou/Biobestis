# Biobestis

A two-device compost bin monitor built on the Seeed XIAO ESP32C3. One device reads sensor data and transmits it over UDP; the other displays the status on a pair of OLED screens.

## Hardware

| Role | Board | Sensors / Peripherals |
|---|---|---|
| Receiver (display) | Seeed XIAO ESP32C3 | SSD1306 OLED ×2 (I²C via D4/D5), DS1302 RTC (D6/D7/D8), reset button (D1) |
| Transmitter (sensor) | Seeed XIAO ESP32C3 | DHT22 temperature/humidity sensor (D10) |

## How it works

The **transmitter** reads humidity and temperature from a DHT22 and sends `SensorPacket` UDP datagrams to the receiver on port 4210.

The **receiver** displays three rotating screens on the data OLED (cycling every 10 s):
- Fullness level (0–100 %) with a progress bar
- Humidity (%)
- Days since the bin was last emptied

A second "eye" OLED shows an animated status face whose expression reflects the worst of the three metrics. The overall status is bucketed into six levels: Great → Good → OK → Meh → Bad → Critical.

The DS1302 RTC persists the "last emptied" timestamp across power cycles (stored in RTC RAM). Pressing the reset button on D1 marks the bin as emptied and resets fullness to 0.

Both devices connect to the same WiFi network defined in [src/shared/protocol.h](src/shared/protocol.h).

## Project structure

```
src/
  receiver/       # Display device firmware
  transmitter/    # Sensor device firmware
  shared/         # protocol.h — shared packet format and WiFi credentials
include/          # Additional headers (eye_display, data_display, wifi_controller, tca9548a)
serial_cli.py     # Python CLI for manually sending commands to the receiver over serial
```

## Building and flashing

Requires [PlatformIO](https://platformio.org/).

```bash
# Flash the receiver
pio run -e receiver -t upload

# Flash the transmitter
pio run -e transmitter -t upload

# Open serial monitor
pio device monitor -e receiver
```

## Serial CLI

`serial_cli.py` connects to the receiver over USB serial and lets you update state manually.

```bash
python3 serial_cli.py
```

Available commands:

| Command | Description |
|---|---|
| `fullness <0-100>` | Set bin fullness percentage |
| `humidity <0-100>` | Set humidity reading |
| `day` | Increment days since emptied by 1 |
| `day <n>` | Set days since emptied to n |
| `now` | Sync RTC to current system time |
| `settime YYYY-MM-DD HH:MM:SS` | Set RTC to a specific time |
| `time` | Print current RTC time |
| `status` | Print all current values |

## Dependencies

| Library | Used by |
|---|---|
| Adafruit SSD1306 | receiver |
| makuna/RTC (DS1302) | receiver |
| Adafruit DHT sensor library | transmitter |
| Adafruit Unified Sensor | transmitter |
