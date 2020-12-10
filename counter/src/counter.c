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
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

//display_config configuration
#define PIN_SDA 21
#define PIN_SCL 22

static void periodic_timer_callback(void* arg);

static const char* TAG = "example";
int beats_per_measure = 4;
int subbeats_per_beat = 1;
int beat_denomination = 4;
float beats_per_minute = 120.0;
int seconds_per_minute = 60;
long useconds_per_second = 1000000;


int COUNTER = 0;

void printValue(u8g2_t *u8g2) {
 	char buf[256];

 	u8g2_ClearBuffer(u8g2);
	u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);
	//u8g2_SetFont(u8g2,u8g2_font_inr27_mn);
        sprintf(buf, "%d", (int) COUNTER);
	u8g2_DrawStr(u8g2, 2, 28, buf);

	u8g2_SendBuffer(u8g2);
}


void display_task (void *pvParameters) {
        // Specify hardware settings
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda = PIN_SDA;
	u8g2_esp32_hal.scl = PIN_SCL;
        // Initialize the hardware
	u8g2_esp32_hal_init(u8g2_esp32_hal);

        u8g2_t u8g2; // a structure which will contain all the data for one display
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(
		&u8g2,
		U8G2_R0,
		//u8x8_byte_sw_i2c,
		u8g2_esp32_i2c_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
       	u8x8_SetI2CAddress(&u8g2.u8x8,0x78);
        u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
        u8g2_SetPowerSave(&u8g2, 0); // wake up display

        int CACHED_COUNTER;

        while (1) {
            CACHED_COUNTER = COUNTER;
            vTaskDelay(50 / portTICK_RATE_MS);
            if (CACHED_COUNTER != COUNTER) {
                printValue(&u8g2);
            }
        }
        vTaskDelete(NULL);
}

void app_main(void)
{
    // Setup Maths
    int64_t subbeat_duration_usec;
    int64_t measure_duration_usec;
    int64_t next_beat_time;
    subbeat_duration_usec = seconds_per_minute * useconds_per_second / beats_per_minute / subbeats_per_beat ;
    measure_duration_usec = subbeat_duration_usec * beats_per_measure / subbeats_per_beat;
    next_beat_time = (int64_t) subbeat_duration_usec;

    ESP_LOGI(TAG, "subbeat_duration_usec value: %lld \n", subbeat_duration_usec);
    /* Create one time:
     * 1. a periodic timer which will run every 0.5s, and print a message
     */

    xTaskCreate(display_task, "displaying", 4096, NULL, 5, NULL);

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */

    /* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 50000000));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());
    

    // Counter loop
    for (int i = 0; i < 64;) {
        int64_t current_time = esp_timer_get_time();
        if (next_beat_time > current_time){
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        else {
            ESP_LOGI(TAG, "Beat Count: %d \nCurrent Time: %lld \nNext Beat Time: %lld", i, current_time, next_beat_time);
            COUNTER = i;
            next_beat_time = next_beat_time + subbeat_duration_usec; 
            ++i;
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

