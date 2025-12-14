# esp32-max6675-6sensor

ESP-IDF project to read up to 6x MAX6675 thermocouple modules using two SPI hosts (3 sensors per host).

## Features
- Up to 6 sensors (K-type via MAX6675)
- Two SPI hosts (SPI2_HOST + SPI3_HOST)
- Config-driven sensor table (easy to switch 1..6 sensors)
- Optional CS guard to avoid floating CS when extra modules are physically connected
- Optional simple averaging filter
- Output units: C / F / K

## Wiring overview
Each MAX6675 uses:
- SCK
- MISO (SO)
- CS (one per module)
- 3V3 + GND

Notes:
- MAX6675 does not use MOSI.
- All grounds must be common.

## User configuration (main/main.c)
Edit only the "User configuration" section.

### SPI hosts
- BUS_A_* : SPI2_HOST pins
- BUS_B_* : SPI3_HOST pins

### Chip Select (CS)
Define CS pins once:
- CS_T1 .. CS_T6

Then assign sensors in `sensor_cfg[]`:
- You can set NUM_SENSORS to 1..6
- You can mix sensors across BUS_A_HOST and BUS_B_HOST

### CS guard (recommended)
If you physically have more MAX6675 modules connected than `NUM_SENSORS`,
their CS pins may float and interfere with the bus.

This project sets all known CS pins to HIGH at startup (`cs_guard_init()`).
Keep `all_cs_pins[]` aligned with CS_T1..CS_T6.

### Units
Set:
- TEMP_UNIT = UNIT_CELSIUS / UNIT_FAHRENHEIT / UNIT_KELVIN

### Filter and sampling
The simple averaging filter reads each sensor multiple times:
- FILTER_SAMPLES
- FILTER_SAMPLE_DELAY_MS

Total time per loop depends on:
- NUM_SENSORS * FILTER_SAMPLES * FILTER_SAMPLE_DELAY_MS
plus
- MEASUREMENT_PERIOD_MS

For 1 Hz logging, keep the filter small.

## Build and flash
```bash
idf.py set-target esp32
idf.py build flash monitor
```