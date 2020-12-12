#ifndef FUNCTIONS_DISPLAY_INCLUDED
#define FUNCTIONS_DISPLAY_INCLUDED
#include "esp_log.h"
#include "driver/gpio.h"

// Gate out GPIO Setup
#define GPIO_OUTPUT_IO_0   25
#define GPIO_OUTPUT_IO_1   26
#define GPIO_OUTPUT_IO_2   4
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2))
#define ESP_INTR_FLAG_DEFAULT 0

void gate_setup(void);

#endif
