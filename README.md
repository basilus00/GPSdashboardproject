🚗 GPS Vehicle Tracking Dashboard
[An Elementary IoT Project for Robotics & Automotive Students]

![GPS Dashboard](images/dashboard.png)

## 📋 Project Overview

This is a **beginner-friendly IoT project** designed to teach robotics and automotive students how to:
- ✅ Track vehicle location in **real-time** on an interactive map
- ✅ Display GPS coordinates, altitude, and satellite information
- ✅ Calculate journey distance and route history
- ✅ Connect hardware (GPS module) to cloud via MQTT
- ✅ Create a professional web dashboard with live data


## 🎯 Learning Objectives

By completing this project, students will learn:

| Objective | What You'll Understand |
|-----------|------------------------|
| **GPS Technology** | How satellites calculate position (latitude, longitude, altitude) |
| **IoT Communication** | MQTT protocol for real-time data transmission |
| **Embedded Systems** | ESP32 microcontroller programming and sensor integration |
| **Web Development** | HTML, CSS, JavaScript for interactive dashboards |
| **Geolocation APIs** | Leaflet.js maps and OpenStreetMap integration |
| **Data Visualization** | Real-time data display and map markers |

---

## 🛠️ Hardware Required (Elementary Level)
ESP32 DevKit + NEO-6M GPS Module + wires + mqqt broker on pc

![GPS_wiring](images/gpswiring.png)

## ✨ How it works
SATELLITE CONSTELLATION (Provides GPS coordinates to module)>> NEO-6M GPS MODULE (RX=16, TX=17, 9600bd) >> ESP32 Microcontroller (Reads GPS data+Processes coordinates+Publishes via MQTT (WiFi)) >> Mosquitto MQTT Broker (localhost:1883) >> (WebSocket) >> Web Browser Dashboard(Leaflet Map ++ Real-time markers ++ GPS data cards ++ Journey tracking)

## 📡 GPS Information Cards (Below Map)
![GPS Cards](images/gps-cards.png)

📍 LATITUDE - Vertical Position
📍 LONGITUDE - Horizontal Position
📏 ALTITUDE - Height Above Sea Level
🛰️ SATELLITES - Signal Quality
📡 HDOP (Accuracy) - Precision Level
⏰ LAST UPDATE - Data Freshness

## 💻 Software Components
--Arduino Code (ESP32) - ESP32_GPS_MQTT.ino
--2. HTML File - index_Version3.html
--CSS Styling - style.css
--JavaScript - script.js


PS:after setting the whole environment you need to activate the mosquitto.exe with you configuartion file mosq.conf
PS:you need also to open dashboard via python -m http.server 8000

## 📝 Code Structure Summary
Project/
├── ESP32_GPS_MQTT.ino        (Arduino firmware)
├── index_Version3.html        (Dashboard interface)
├── style.css                  (Visual styling)
├── script.js                  (Real-time updates)
├── mosq.conf                  (MQTT broker config)
└── README.md                  (This file!)

## 📄 License
MIT License - Free to use and modify for educational purposes

## 👨‍💻 Author
Created for Robotics & Automotive Education Programs

Happy Tracking! 🚗📍🗺️
