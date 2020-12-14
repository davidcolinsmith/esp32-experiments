/* esp_timer (high resolution timer) example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_log.h"
#include "driver/gpio.h"
#include "local_gates.h"
#include "esp_timer.h"

// Gate out GPIO Setup
#define GPIO_OUTPUT_IO_0   25
#define GPIO_OUTPUT_IO_1   26
#define GPIO_OUTPUT_IO_2   4
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2))
#define ESP_INTR_FLAG_DEFAULT 0

void gates_setup(void * pvParameters) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT; //set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0; //disable pull-down mode
    io_conf.pull_up_en = 0; //disable pull-up mode
    gpio_config(&io_conf); //configure GPIO with the given settings

    // int gate1[16] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
    // int gate2[16] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    // int gate3[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    int sequence_length = 16;
    int sequence[sequence_length] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    int sequence_counter = 1;

    while(1) {
        int64_t current_time = esp_timer_get_time();
        // Not time for next beat yet
        if (NEXT_BEAT_TIME > current_time) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // Next beat step is here, play something
        else {
            // Set GPIO level to sequences step value, either high or low
            gpio_set_level(GPIO_OUTPUT_IO_0, sequence[sequence_counter]);
            // Wait one MS
            vTaskDelay(10 / portTICK_PERIOD_MS);
            // Set GPIO low
            gpio_set_level(GPIO_OUTPUT_IO_0, 0);
            if (sequence_counter == sequence_length) {
                sequence_counter = 1;
                //TODO look for update to sequence here, update sequence[]
            }
            else {
                ++sequence_counter;
            }
}

