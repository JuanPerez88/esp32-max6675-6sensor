#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#include "max6675.h"
#include "driver/gpio.h"

// =========================
// User configuration
// =========================

#define SPI_CLOCK_HZ            1000000
#define MEASUREMENT_PERIOD_MS   1000

#define BUS_A_HOST              SPI2_HOST
#define BUS_A_MISO              19
#define BUS_A_SCK               18
#define BUS_A_MOSI              -1

#define BUS_B_HOST              SPI3_HOST
#define BUS_B_MISO              27
#define BUS_B_SCK               14
#define BUS_B_MOSI              -1

#define CS_T1  5
#define CS_T2  17
#define CS_T3  16
#define CS_T4  26
#define CS_T5  25
#define CS_T6  33

#define UNIT_CELSIUS     0
#define UNIT_FAHRENHEIT  1
#define UNIT_KELVIN      2

#define TEMP_UNIT               UNIT_CELSIUS

#define FILTER_SAMPLES          4
#define FILTER_SAMPLE_DELAY_MS  5

#define NUM_SENSORS             6

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
    spi_host_device_t host;
    int cs_pin;
    const char *name;
} max6675_sensor_cfg_t;

static const max6675_sensor_cfg_t sensor_cfg[NUM_SENSORS] = {
    { BUS_A_HOST, CS_T1, "T1" },
    { BUS_A_HOST, CS_T2, "T2" },
    { BUS_A_HOST, CS_T3, "T3" },
    { BUS_B_HOST, CS_T4, "T4" },
    { BUS_B_HOST, CS_T5, "T5" },
    { BUS_B_HOST, CS_T6, "T6" },
};

static const int all_cs_pins[] = { CS_T1, CS_T2, CS_T3, CS_T4, CS_T5, CS_T6 };

// =========================

static spi_device_handle_t sensors[NUM_SENSORS];

static float convert_unit(float c)
{
#if TEMP_UNIT == UNIT_CELSIUS
    return c;
#elif TEMP_UNIT == UNIT_FAHRENHEIT
    return (c * 9.0f / 5.0f) + 32.0f;
#elif TEMP_UNIT == UNIT_KELVIN
    return c + 273.15f;
#else
    return c;
#endif
}

static const char *unit_suffix(void)
{
#if TEMP_UNIT == UNIT_CELSIUS
    return "C";
#elif TEMP_UNIT == UNIT_FAHRENHEIT
    return "F";
#elif TEMP_UNIT == UNIT_KELVIN
    return "K";
#else
    return "C";
#endif
}

static float read_filtered_celsius(spi_device_handle_t dev)
{
    float sum = 0.0f;

    for (int i = 0; i < FILTER_SAMPLES; i++) {
        float t = max6675_read_celsius(dev);
        if (isnan(t)) {
            return NAN;
        }
        sum += t;
        vTaskDelay(pdMS_TO_TICKS(FILTER_SAMPLE_DELAY_MS));
    }

    return sum / (float)FILTER_SAMPLES;
}

static bool any_sensor_on_host(spi_host_device_t host)
{
    for (int i = 0; i < NUM_SENSORS; i++) {
        if (sensor_cfg[i].host == host) {
            return true;
        }
    }
    return false;
}

static esp_err_t init_buses(void)
{
    if (any_sensor_on_host(BUS_A_HOST)) {
        esp_err_t err = max6675_bus_init_host(BUS_A_HOST, BUS_A_MISO, BUS_A_MOSI, BUS_A_SCK, SPI_CLOCK_HZ);
        if (err != ESP_OK) {
            return err;
        }
    }

    if (any_sensor_on_host(BUS_B_HOST)) {
        esp_err_t err = max6675_bus_init_host(BUS_B_HOST, BUS_B_MISO, BUS_B_MOSI, BUS_B_SCK, SPI_CLOCK_HZ);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

static esp_err_t init_sensors(void)
{
    esp_err_t err = init_buses();
    if (err != ESP_OK) {
        return err;
    }

    for (int i = 0; i < NUM_SENSORS; i++) {
        err = max6675_add_sensor_host(sensor_cfg[i].host, sensor_cfg[i].cs_pin, &sensors[i]);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

static void cs_guard_init(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    for (int i = 0; i < (int)ARRAY_LEN(all_cs_pins); i++) {
        io_conf.pin_bit_mask = 1ULL << all_cs_pins[i];
        gpio_config(&io_conf);
        gpio_set_level(all_cs_pins[i], 1);
    }
}

void app_main(void)
{
    cs_guard_init();

    if (init_sensors() != ESP_OK) {
        printf("MAX6675 init failed\n");
        return;
    }

    while (1) {
        for (int i = 0; i < NUM_SENSORS; i++) {
            float c = read_filtered_celsius(sensors[i]);
            float v = convert_unit(c);

            if (isnan(v)) {
                printf("%s: nan %s\n", sensor_cfg[i].name, unit_suffix());
            } else {
                printf("%s: %.2f %s\n", sensor_cfg[i].name, v, unit_suffix());
            }
        }

        printf("----\n");
        vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_PERIOD_MS));
    }
}
