# GPS Dashboard Project

A real-time location monitoring system featuring an interactive Leaflet map, live coordinate display, satellite tracking, accuracy metrics, distance calculation, and journey history visualization — powered by an MQTT-connected ESP32 GPS module.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
  - [ESP32 Firmware Setup](#esp32-firmware-setup)
  - [MQTT Broker Setup](#mqtt-broker-setup)
  - [Dashboard Setup](#dashboard-setup)
- [Configuration](#configuration)
- [Usage](#usage)
- [MQTT Payload Format](#mqtt-payload-format)
- [Screenshots](#screenshots)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

The GPS Dashboard Project provides a web-based interface for real-time GPS tracking using an ESP32 microcontroller paired with a GPS module. Location data is published over MQTT and consumed by a browser dashboard that renders the position on an interactive map, tracks journey history, and displays satellite and accuracy statistics.

---

## Features

- 🗺️ **Interactive Leaflet Map** — Real-time marker updates with zoom and pan support
- 📍 **Live Coordinate Display** — Latitude and longitude shown with high precision
- 🛰️ **Satellite Tracking** — Number of satellites in view displayed in real time
- 🎯 **Accuracy Metrics** — HDOP / fix accuracy indicator to assess GPS signal quality
- 📏 **Distance Calculation** — Cumulative distance traveled computed automatically
- 🛤️ **Journey History Visualization** — Polyline trace of the full route on the map
- 📡 **MQTT Integration** — Lightweight, real-time data transport from the ESP32
- 🔄 **Auto-Reconnect** — Automatic reconnection to the MQTT broker on connection loss

---

## System Architecture

```
┌─────────────────────┐        MQTT         ┌────────────────────┐
│  ESP32 + GPS Module │ ──────────────────▶ │   MQTT Broker      │
│  (publishes GPS     │                     │  (e.g. Mosquitto)  │
│   data as JSON)     │                     └────────┬───────────┘
└─────────────────────┘                              │
                                                     │ WebSocket / MQTT
                                                     ▼
                                        ┌────────────────────────┐
                                        │   GPS Web Dashboard    │
                                        │  (Leaflet + MQTT.js)   │
                                        └────────────────────────┘
```

---

## Hardware Requirements

| Component | Details |
|-----------|---------|
| **Microcontroller** | ESP32 (any variant with Wi-Fi) |
| **GPS Module** | NMEA-compatible module (e.g. NEO-6M, NEO-8M, GT-U7) |
| **Connection** | GPS TX → ESP32 RX (hardware or software serial) |
| **Power** | USB or 3.3 V/5 V regulated supply |

---

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) for ESP32 firmware
- ESP32 Arduino Core
- [TinyGPS++](http://arduiniana.org/libraries/tinygpsplus/) library
- [PubSubClient](https://github.com/knolleary/pubsubclient) (MQTT client for Arduino)
- An MQTT broker (e.g. [Mosquitto](https://mosquitto.org/), [HiveMQ](https://www.hivemq.com/), or a cloud broker)
- A modern web browser (Chrome, Firefox, Edge, Safari)
- [Node.js](https://nodejs.org/) *(optional — only if serving the dashboard locally)*

---

## Getting Started

### ESP32 Firmware Setup

1. Install the **ESP32 Arduino Core** in Arduino IDE (or configure PlatformIO).
2. Install the following libraries via the Library Manager:
   - `TinyGPS++`
   - `PubSubClient`
3. Open the firmware sketch (e.g. `firmware/gps_mqtt.ino`).
4. Update the configuration section with your Wi-Fi credentials and MQTT broker address (see [Configuration](#configuration)).
5. Upload the sketch to your ESP32.
6. Connect the GPS module's TX pin to the ESP32 RX pin (default: GPIO 16).

### MQTT Broker Setup

You can use any MQTT broker. To run Mosquitto locally:

```bash
# Install Mosquitto
sudo apt install mosquitto mosquitto-clients   # Debian/Ubuntu
brew install mosquitto                          # macOS

# Start the broker
mosquitto -v
```

For WebSocket support (required by the browser dashboard), add the following to your `mosquitto.conf`:

```
listener 9001
protocol websockets
```

### Dashboard Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/basilus00/GPSdashboardproject.git
   cd GPSdashboardproject
   ```
2. Open `index.html` in your browser, **or** serve it with a local server:
   ```bash
   # Using Python
   python3 -m http.server 8080

   # Using Node.js (npx)
   npx serve .
   ```
3. Navigate to `http://localhost:8080` (or the address shown in your terminal).

---

## Configuration

### ESP32 Firmware (`firmware/gps_mqtt.ino`)

```cpp
// Wi-Fi
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT Broker
const char* MQTT_BROKER   = "192.168.1.100";  // IP or hostname
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "gps/location";

// GPS Serial pins (adjust for your wiring)
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
```

### Dashboard (`config.js` or inline in `index.html`)

```javascript
const MQTT_BROKER_URL = "ws://192.168.1.100:9001";  // WebSocket URL
const MQTT_TOPIC      = "gps/location";
```

---

## Usage

1. Power on the ESP32 with the GPS module attached.
2. Wait for the GPS to acquire a fix (the onboard LED may indicate status).
3. Open the dashboard in your browser.
4. The map will automatically center on your current location and begin plotting your route.

**Dashboard panels:**

| Panel | Description |
|-------|-------------|
| **Map** | Live position marker and route polyline on an OpenStreetMap base layer |
| **Coordinates** | Current latitude and longitude |
| **Satellites** | Number of satellites used in the current fix |
| **Accuracy** | HDOP value — lower is better (< 2 is excellent) |
| **Distance** | Total distance traveled since the dashboard was opened |
| **Journey History** | Full polyline trace of the journey on the map |

---

## MQTT Payload Format

The ESP32 publishes GPS data as a JSON object to the configured topic:

```json
{
  "lat": 51.5074,
  "lng": -0.1278,
  "alt": 35.2,
  "speed": 12.4,
  "course": 270.0,
  "satellites": 8,
  "hdop": 1.2,
  "timestamp": "2026-03-19T13:45:00Z"
}
```

| Field | Type | Description |
|-------|------|-------------|
| `lat` | float | Latitude in decimal degrees |
| `lng` | float | Longitude in decimal degrees |
| `alt` | float | Altitude in meters |
| `speed` | float | Speed over ground in km/h |
| `course` | float | True course/heading in degrees |
| `satellites` | int | Number of satellites in use |
| `hdop` | float | Horizontal dilution of precision |
| `timestamp` | string | ISO 8601 UTC timestamp |

---

## Screenshots

> *(Add screenshots of your running dashboard here)*

---

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m "Add my feature"`
4. Push to the branch: `git push origin feature/my-feature`
5. Open a Pull Request

---

## License

This project is open-source. See the [LICENSE](LICENSE) file for details.

