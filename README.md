# 🌿 AI & IoT Based Greenhouse Monitoring System

## 📋 Overview

This project is a smart greenhouse monitoring system that uses **ESP32**, various sensors, and **Firebase** to monitor and control the greenhouse environment in real-time. The system automates decision-making based on environmental conditions and provides live data visualization through a web dashboard.

---

## 🚀 Features

- 🌡️ Temperature and Humidity Monitoring (DHT11)
- 🌱 Soil Moisture Monitoring
- 🧪 Air Quality Monitoring (MQ-135 Gas Sensor)
- 💧 Water Availability Detection
- 💨 Fan Control based on temperature
- 🚿 Pump Control based on soil moisture and water level
- ☁️ Real-time data upload to Firebase
- 🌐 Live monitoring dashboard (HTML + Bootstrap)
- 🔋 Low power embedded design with L298N motor driver

---

## 🔧 Hardware Used

| Component           | Purpose                              |
|---------------------|--------------------------------------|
| ESP32-WROOM-32      | Microcontroller                      |
| DHT11               | Temperature & Humidity Sensor        |
| MQ-135              | Gas Sensor (Air Quality)             |
| Soil Moisture Sensor| Soil Moisture Detection              |
| Water Level Sensor  | Detects Water Presence in Tank       |
| L298N Motor Driver  | Controls Fan and Pump Motors         |
| 12V DC Fan & Pump   | Actuators for cooling & irrigation   |
| 7.4V 3A Battery     | Power Source                         |

---

## 💻 Software Used

| Software           | Description                                      |
|--------------------|--------------------------------------------------|
| PlatformIO (VS Code)| Embedded development environment for ESP32      |
| Firebase Realtime DB| Cloud database for storing sensor data          |
| Firebase Hosting    | Hosting the live dashboard                      |
| Arduino Libraries   | For DHT11, MQ-135, etc.                         |
| HTML + Bootstrap    | Front-end live data dashboard                   |
| GitHub              | Version control and project repository          |

---

## 🧠 System Working

1. **Sensor Data Collection**  
   ESP32 reads values from all sensors in real-time.

2. **Decision Logic on ESP32**  
   - Fan activates if temperature > 35°C  
   - Pump activates if soil moisture < 50% and water is present

3. **Data Upload**  
   Sensor data is uploaded to Firebase every 10 seconds.

4. **Live Dashboard**  
   Website fetches data from Firebase and displays it live.

---

## 🌐 Live Dashboard

A responsive and interactive website built using **HTML, Bootstrap, and Firebase JS SDK** allows users to monitor:

- Temperature  
- Humidity  
- Soil Moisture  
- Gas Level  
- Water Status  
- Fan & Pump Status  
- Timestamp of latest update

---

## 📁 Folder Structure

