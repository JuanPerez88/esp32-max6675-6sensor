# esp32-max6675-6sensor

ESP-IDF project to read up to 6x MAX6675 thermocouple modules using two SPI hosts (3 sensors per host).

---

## Overview

Scalable and robust setup using:
- Two SPI hosts (SPI2 + SPI3)
- Declarative sensor configuration
- Protection against floating CS lines

This project assumes a shared MISO line per SPI bus.  
To ensure reliable detection of disconnected or missing MAX6675 modules,
a pull-down resistor on each SPI MISO line is required.

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
- 2 × 10 kΩ resistors (one per SPI MISO line)

---

## Important hardware note (MISO pull-down)

When using multiple MAX6675 modules on a shared SPI bus (shared MISO),
it is **strongly recommended** to add a pull-down resistor on each SPI MISO line.

### Recommended setup

- One **10 kΩ pull-down resistor**
- Connected between **MISO and GND**
- **One resistor per SPI bus** (not per sensor)

Example:
- SPI2 MISO → 10 kΩ → GND
- SPI3 MISO → 10 kΩ → GND

### Why this is required

The MAX6675 does **not actively drive MISO high** when:
- The module is disconnected
- The module is unpowered
- CS is floating
- The module is missing

Without a pull-down resistor, the MISO line may float and produce:
- False temperature readings (e.g. `0.00 C`)
- Saturated values (e.g. `1023.75 C`)
- Cross-contamination between sensors on the same bus

The driver detects floating or invalid SPI data patterns, but a pull-down
resistor ensures deterministic electrical behavior and reliable fault detection.

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

Edit the User configuration section in `main/main.c`:

- SPI hosts and pins
- CS pin definitions
- Sensor table (`sensor_cfg[]`)
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

## Sensor status reference

Each MAX6675 read operation returns a temperature value and a status code.

| Status | Meaning | Typical console output | Notes |
|------|--------|------------------------|------|
| `MAX6675_STATUS_OK` | Valid temperature reading | `21.50 C` | Thermocouple connected and SPI communication OK |
| `MAX6675_STATUS_OPEN_THERMOCOUPLE` | Thermocouple disconnected | `TC OPEN` | One or both thermocouple wires not connected |
| `MAX6675_STATUS_SPI_ERROR` | SPI communication error | `SPI ERR` | Missing module, floating MISO, wiring or power issue |
| `MAX6675_STATUS_INVALID_ARG` | Invalid API usage | `ERR` | Programming error (should not happen in normal use) |

---

## License

MIT License