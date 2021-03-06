/* esp_timer (high resolution timer) example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "util/gates.c"

static void periodic_timer_callback(void* arg);
static void oneshot_timer_callback(void* arg);

static const char* TAG = "example";

void setup() {
    gpio_set_direction(GATEout_0, GPIO_MODE_OUTPUT);
    gpio_set_direction(GATEout_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GATEout_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GATEout_3, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    /* Create two timers:
     * 1. a periodic timer which will run every 0.5s, and print a message
     * 2. a one-shot timer which will fire after 5s, and re-start periodic
     *    timer with period of 1s.
     */

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */

    /* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());

    /* Print debugging information about timers to console every 2 seconds */
    for (int i = 0; i < 10; ++i) {
        ESP_ERROR_CHECK(esp_timer_dump(stdout));
        usleep(2000000);
    }

    /* Timekeeping continues in light sleep, and timers are scheduled
     * correctly after light sleep.
     */
    ESP_LOGI(TAG, "Entering light sleep for 0.5s, time since boot: %lld us",
            esp_timer_get_time());

    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(500000));
    esp_light_sleep_start();

    ESP_LOGI(TAG, "Woke up from light sleep, time since boot: %lld us",
                esp_timer_get_time());

    /* Let the timer run for a little bit more */
    usleep(2000000);

    /* Clean up and finish the example */
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
    ESP_LOGI(TAG, "Stopped and deleted timers");
}

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
}


