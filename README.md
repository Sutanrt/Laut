# IoT Boat Safety System

An IoT-based boat safety monitoring system built using **ESP32** and **PlatformIO**. The system monitors the boat's position and orientation, displays important information locally, provides an audible warning when unsafe conditions are detected, and communicates data between boats using **LoRa**.

## Overview

The Boat Safety System is designed to improve situational awareness and safety for small boats.

The system uses:

* **ESP32** as the main controller.
* **GPS Module** for boat location tracking.
* **MPU6500** for monitoring boat tilt and acceleration.
* **LoRa SX1278 / RA-02** for long-range wireless communication between boats.
* **OLED SSD1306** for displaying system information.
* **Buzzer** for audible safety warnings.

When potentially unsafe boat tilt is detected, the system can activate the buzzer and transmit relevant information to other LoRa-equipped boats.

---

## Hardware

| Component            | Function                    |
| -------------------- | --------------------------- |
| ESP32 DOIT DevKit V1 | Main microcontroller        |
| LoRa SX1278 RA-02    | Wireless communication      |
| GPS Module           | Position tracking           |
| MPU6500              | Accelerometer and gyroscope |
| SSD1306 OLED         | Local information display   |
| Buzzer               | Audible warning             |

---

## Pin Configuration

### I2C

Used by the OLED and MPU6500.

| Pin |   ESP32 |
| --- | ------: |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### OLED SSD1306

* I2C Address: `0x3C`

### MPU6500

* I2C Address: `0x68`
* Expected `WHO_AM_I`: `0x70`

### GPS

| GPS |              ESP32 |
| --- | -----------------: |
| TX  | GPIO 16 / ESP32 RX |
| RX  | GPIO 17 / ESP32 TX |

Default baud rate:

```text
9600
```

### LoRa SX1278 RA-02

| LoRa RA-02 |   ESP32 |
| ---------- | ------: |
| SCK        | GPIO 18 |
| MISO       | GPIO 19 |
| MOSI       | GPIO 23 |
| NSS / SS   |  GPIO 5 |
| RST        | GPIO 14 |
| DIO0       | GPIO 26 |

Operating frequency:

```text
433 MHz
```

### Buzzer

```text
GPIO 25
```

---

## Main Features

### Boat Position Monitoring

The GPS module reads the boat's geographical position in the form of latitude and longitude.

The position can be displayed on the OLED and transmitted to another boat through LoRa.

### Boat Tilt Monitoring

The MPU6500 measures acceleration and angular movement to estimate the orientation and tilt of the boat.

The sensor is configured with an accelerometer range of approximately:

```text
±8g
```

### LoRa Communication

The RA-02 module provides long-range communication between devices without requiring cellular or internet connectivity.

The current system is designed around:

```text
433 MHz
```

Data such as GPS position and safety status can be transmitted periodically between boats.

Example transmission interval:

```text
2 seconds
```

### Safety Warning

The buzzer provides an audible indication of the current safety condition.

Example behavior:

* **Safe condition:** short or intermittent beep.
* **Unsafe condition:** long warning tone.

### OLED Display

The OLED can display information such as:

* System status
* GPS status
* Latitude
* Longitude
* Boat tilt
* LoRa status
* Safety condition

---

## Development Environment

This project is developed using:

* **Visual Studio Code**
* **PlatformIO**
* **Arduino Framework**
* **ESP32 DOIT DevKit V1**

---

## Project Structure

Typical PlatformIO project structure:

```text
Laut/
├── include/
├── lib/
├── src/
│   └── main.cpp
├── test/
├── .gitignore
├── platformio.ini
└── README.md
```

The primary program is located in:

```text
src/main.cpp
```

PlatformIO configuration is stored in:

```text
platformio.ini
```

---

## Installation

### 1. Install Visual Studio Code

Install Visual Studio Code and the **PlatformIO IDE** extension.

### 2. Clone the Repository

```bash
git clone https://github.com/Sutanrt/Laut.git
```

Move into the project directory:

```bash
cd Laut
```

### 3. Open with VS Code

Open the project folder in Visual Studio Code.

PlatformIO should automatically recognize the project from:

```text
platformio.ini
```

### 4. Connect ESP32

Connect the ESP32 to the computer through USB.

### 5. Build the Project

Using PlatformIO:

```bash
pio run
```

### 6. Upload to ESP32

```bash
pio run --target upload
```

### 7. Open Serial Monitor

```bash
pio device monitor
```

Serial baud rate:

```text
115200
```

---

## System Flow

```text
              ┌─────────────┐
              │     GPS     │
              └──────┬──────┘
                     │
                     ▼
┌─────────────┐   ┌─────────────┐   ┌─────────────┐
│   MPU6500   │──▶│    ESP32    │──▶│ OLED Display│
└─────────────┘   └──────┬──────┘   └─────────────┘
                         │
              ┌──────────┴──────────┐
              │                     │
              ▼                     ▼
        ┌─────────────┐       ┌─────────────┐
        │   Buzzer    │       │ LoRa RA-02  │
        └─────────────┘       └──────┬──────┘
                                    │
                                    ▼
                              Other Boat
```

---

## Communication Concept

Each boat can be equipped with an ESP32 and LoRa module.

Example communication:

```text
Boat 1
ESP32 + GPS + MPU6500
        │
        │ LoRa 433 MHz
        ▼
Boat 2
ESP32 + LoRa
```

A transmitted packet may contain information such as:

```text
Boat ID
Latitude
Longitude
Tilt
Safety Status
```

Example:

```text
BOAT01,-6.095208,106.149451,12.5,SAFE
```

---

## Serial Debugging

The project uses:

```text
115200 baud
```

Typical debugging messages may include:

```text
Boat Safety System Starting...
MPU6500 berhasil!
Waiting GPS...
GPS FIX
LoRa berhasil start!
```

These messages can be viewed through PlatformIO Serial Monitor.

---

## Important Notes

The MPU6500 is used for measuring **acceleration, rotation, and boat orientation**. It does not directly measure the boat's actual travelling speed.

Boat speed should instead be obtained from GPS data if required.

The effective LoRa communication range depends strongly on antenna quality, antenna placement, obstacles, interference, transmission power, and environmental conditions.

An appropriate **433 MHz antenna** should be connected to the RA-02 module before transmission.

---

## Future Development

Possible improvements include:

* Emergency distress message transmission.
* Boat-to-boat warning system.
* GPS-based boat speed estimation.
* Automatic detection of dangerous tilt angles.
* Multiple boat identification.
* LoRa gateway integration.
* Data logging.
* Web or mobile monitoring dashboard.
* Battery monitoring.
* SOS button.
* Improved communication protocol with packet acknowledgement.

---

## Repository

GitHub:

```text
https://github.com/Sutanrt/Laut
```

---

## License

This project is currently intended for educational and research purposes.
