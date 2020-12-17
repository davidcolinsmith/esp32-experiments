/* esp_timer (high resolution timer) example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
//#include <stdio.h>
//#include <stdlib.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "sequencer.h"
#include "esp_timer.h"

// Gate out GPIO Setup
#define GPIO_OUTPUT_IO_0   25
#define GPIO_OUTPUT_IO_1   26
#define GPIO_OUTPUT_IO_2   4
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2))
#define ESP_INTR_FLAG_DEFAULT 0

extern int64_t NEXT_BEAT_TIME;

static const char* TAG = "Sequencer";

void sequencer_task(void * pvParameters) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT; //set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0; //disable pull-down mode
    io_conf.pull_up_en = 0; //disable pull-up mode
    gpio_config(&io_conf); //configure GPIO with the given settings

    int SIZE = 5;
    int SEQUENCE[5] = {1, 2, 3, 4, 5};
    int COUNTER = 0;

    while(1) {
        int64_t current_time = esp_timer_get_time();
        // Not time for next beat yet
        if (NEXT_BEAT_TIME > current_time) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        // Next beat step is here, play something
        else {
            int64_t drift = current_time - NEXT_BEAT_TIME;
            ESP_LOGI(TAG, "Sequence Value: %d Drift: %lld", SEQUENCE[COUNTER], drift);
            // Set GPIO level to sequences step value, either high or low
            gpio_set_level(GPIO_OUTPUT_IO_0, SEQUENCE[COUNTER]);
            // Wait one MS
            vTaskDelay(10 / portTICK_PERIOD_MS);
            // Set GPIO low
            gpio_set_level(GPIO_OUTPUT_IO_0, 0);
            // Pop the played character off of the sequence
            COUNTER++;
            if (COUNTER == (SIZE)) {
                COUNTER = 0;
                //TODO look for update to sequence here, update sequence[]
            }
        }
    }
}
