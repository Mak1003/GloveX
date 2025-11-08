<!-- ========================================== -->
<!--        GLOVE X+ - SMART GLOVE README       -->
<!-- ========================================== -->

<h1 align="center">🧤 GloveX+ — AI + IoT Smart Glove for Gesture Communication</h1>

<p align="center">
  <b>Empowering the speech and hearing impaired through IoT, AI, and innovation.</b><br>
  Translating hand gestures into meaningful speech — bridging human communication barriers.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/ESP32-Microcontroller-blue?style=flat-square" />
  <img src="https://img.shields.io/badge/IoT-Enabled-orange?style=flat-square" />
  <img src="https://img.shields.io/badge/AI%20Integration-Ready-yellow?style=flat-square" />
  <img src="https://img.shields.io/badge/Status-Active-success?style=flat-square" />
  <img src="https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square" />
</p>

---


## 🚀 Overview

**GloveX+** is a next-generation assistive technology designed to help **speech- and hearing-impaired individuals** communicate effortlessly using **hand gestures**.  
Built with **ESP32**, **Flex Sensors**, and **MPU6050**, the glove detects real-time finger bends and motion, sending data to a **Python-based Streamlit web app** that **speaks the detected gesture aloud**.

🧠 It supports **32+ mapped gesture combinations**, along with dynamic motion detection for “move”, “yes/no”, and “help” commands.  
GloveX+ aims to bridge the communication gap using **IoT + AI + empathy-driven innovation**.

---

## ✨ Features

| Feature | Description |
|----------|-------------|
| 🖐 **Gesture Detection** | Detects finger bend and wrist orientation in real-time using flex sensors + MPU6050. |
| 🔊 **Text-to-Speech Conversion** | Instantly converts recognized gestures into spoken words using Python `pyttsx3`. |
| 🌐 **Streamlit Web Interface** | Displays live gesture feed, auto-speech output, and gesture history. |
| 📡 **Wireless Communication** | Uses ESP32 Bluetooth and Serial data streaming for real-time updates. |
| 🔋 **Portable Design** | Fully rechargeable via TP4056 + Li-ion battery module. |
| 🧩 **AI Integration Ready** | Designed for future GPT/Google AI-based sentence generation. |
| 🏠 **IoT Compatibility** | Can control appliances (lights, fans, etc.) using gestures in future upgrades. |

---

## ⚙️ System Architecture
[FLEX SENSORS + MPU6050]->
[ESP32 MCU]-> Bluetooth / Serial-> [Python Streamlit Web App]->
[Speech Output via TTS]


---

## 🧠 Hardware Components

| Component | Purpose |
|------------|----------|
| **ESP32 DevKit** | Central microcontroller handling sensor inputs and Bluetooth communication |
| **Flex Sensors (x5)** | Detects bending of each finger for gesture mapping |
| **MPU6050 Sensor** | Measures motion and orientation (pitch, roll, yaw) |
| **TP4056 Module** | Enables battery charging and protection |
| **3.7V Li-ion Battery** | Portable power supply for glove |
| **LED (GPIO 2)** | Visual indicator for gesture recognition |

---

## 🔌 Circuit Connections

| Component | ESP32 Pin | Description |
|------------|------------|-------------|
| Flex Sensor 1 | GPIO 34 | Thumb |
| Flex Sensor 2 | GPIO 35 | Index |
| Flex Sensor 3 | GPIO 32 | Middle |
| Flex Sensor 4 | GPIO 33 | Ring |
| Flex Sensor 5 | GPIO 25 | Little Finger |
| MPU6050 SDA | GPIO 21 | I2C Data |
| MPU6050 SCL | GPIO 22 | I2C Clock |
| LED | GPIO 2 | Status LED |
| Battery (TP4056 OUT+/OUT-) | VIN / GND | Power input |

---

## 💻 Software Stack

| Layer | Technology | Description |
|--------|-------------|-------------|
| **Firmware** | C++ (Arduino) | Handles flex + motion reading, gesture logic, and Bluetooth output |
| **Web App** | Python (Streamlit) | Visualizes gesture feed, converts text to speech, logs history |
| **Speech Engine** | pyttsx3 | Provides offline text-to-speech conversion |
| **Communication** | pyserial | Reads serial data from ESP32 continuously |

---

## 🛠️ Installation & Setup

### 1️⃣ Upload ESP32 Code
1. Open the `glovex_esp32.ino` file in Arduino IDE.  
2. Select **Board:** ESP32 Dev Module.  
3. Select your **COM port**.  
4. Click **Upload**.

---

### 2️⃣ Setup Streamlit App
```bash
# Create Virtual Environment (Recommended)
python -m venv .venv
.venv\Scripts\activate

# Install dependencies
pip install streamlit pyserial pyttsx3

# Run the app
streamlit run app.py

