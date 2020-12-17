/* esp_timer (high resolution timer) example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "nvs_flash.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include "lwip/err.h"
#include "lwip/sys.h"

#include "display.h"
#include "gate.h"
#include "wifi.h"
#include "osc.h"


// Gate out GPIO Setup
#define GPIO_OUTPUT_IO_0   25
#define GPIO_OUTPUT_IO_1   26
#define GPIO_OUTPUT_IO_2   4
//#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2))
//#define ESP_INTR_FLAG_DEFAULT 0

static void periodic_timer_callback(void* arg);

static const char* TAG = "example";
int beats_per_measure = 4;
int subbeats_per_beat = 1;
int beat_denomination = 4;
float beats_per_minute = 240.0;
int seconds_per_minute = 60;
long useconds_per_second = 1000000;

int COUNTER = 1;

void app_main(void)
{

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // Start UDP listener
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);


    // Setup Maths
    int64_t subbeat_duration_usec;
    int64_t measure_duration_usec;
    int64_t next_beat_time;
    subbeat_duration_usec = seconds_per_minute * useconds_per_second / beats_per_minute / subbeats_per_beat ;
    // measure_duration_usec = subbeat_duration_usec * beats_per_measure / subbeats_per_beat;
    next_beat_time = (int64_t) subbeat_duration_usec;
    ESP_LOGI(TAG, "subbeat_duration_usec value: %lld \n", subbeat_duration_usec);

    // Start display listener
    xTaskCreate(display_task, "displaying", 4096, NULL, 5, NULL);

    // Setup timer for tempo reference
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    // Start the timer
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 50000000));
    ESP_LOGI(TAG, "Started timers time since boot: %lld us", esp_timer_get_time());
    
    // GPIO initialization
    // gate_setup();
    // Drum sequence build
    int kick[16] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
    int snare[16] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    int hat[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    // Counter loop
    while (1) {

        int64_t current_time = esp_timer_get_time();
        //ESP_LOGI(TAG, "COUNTER: %d \n", COUNTER);
        if (next_beat_time > current_time){
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        else {
            next_beat_time = next_beat_time + subbeat_duration_usec; 
            gpio_set_level(GPIO_OUTPUT_IO_0, kick[COUNTER]);
            gpio_set_level(GPIO_OUTPUT_IO_1, snare[COUNTER]);
            gpio_set_level(GPIO_OUTPUT_IO_2, hat[COUNTER]);
            vTaskDelay(10 / portTICK_RATE_MS);
            gpio_set_level(GPIO_OUTPUT_IO_0, 0);
            gpio_set_level(GPIO_OUTPUT_IO_1, 0);
            gpio_set_level(GPIO_OUTPUT_IO_2, 0);
            if ( COUNTER == 16) {
                COUNTER = 1;
            }
            else {
                ++COUNTER;
            }
    }
    }
    /* Clean up */
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
}

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
}

