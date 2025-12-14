# esp32-max6675-6sensor

ESP-IDF project to read up to 6x MAX6675 thermocouple modules using two SPI hosts (3 sensors per host).

---

## Overview

Scalable and robust setup using:
- Two SPI hosts (SPI2 + SPI3)
- Declarative sensor configuration
- Protection against floating CS lines

---

## Features

- Up to 6 MAX6675 sensors
- Two SPI hosts
- Config-driven sensor table
- CS guard for unused / extra modules
- Optional averaging filter
- Output units: C / F / K

---

## Hardware requirements

- ESP32 development board
- 1–6 × MAX6675 modules
- Type-K thermocouples
- USB cable

---

## Project structure

- `main/main.c`  
  Example application

- `components/max6675/`  
  MAX6675 driver (git submodule)

---

## Getting started

### 1. Prerequisites

- ESP-IDF installed and working

### 2. Clone the repository

```bash
git clone --recurse-submodules https://github.com/JuanPerez88/esp32-max6675-6sensor.git
cd esp32-max6675-6sensor
```

### 3. Configure

Edit the User configuration section in main/main.c:

- SPI hosts and pins
- CS pin definitions
- Sensor table (sensor_cfg[])
- Units and filter parameters

### 4. Build

```bash
idf.py set-target esp32
idf.py build
```

### 5. Flash and monitor

```bash
idf.py flash monitor
```

---

## Expected output

```makefile
T1: 21.00 C
T2: 21.75 C
T3: 23.00 C
T4: 21.25 C
T5: 21.00 C
T6: 21.50 C
```
---

## License

MIT License