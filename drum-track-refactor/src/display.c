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
#include "u8g2_esp32_hal.h"
#include "driver/gpio.h"
#include "display.h"

//display_config configuration
#define PIN_SDA 21
#define PIN_SCL 22


static const char* TAG = "example";

void printValue(u8g2_t *u8g2) {
 	char buf[256];

        ESP_LOGI(TAG, "printValue COUNTER: %d \n", COUNTER);
 	u8g2_ClearBuffer(u8g2);
	u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);
	//u8g2_SetFont(u8g2,u8g2_font_inr27_mn);
        sprintf(buf, "%d", (int) COUNTER);
	u8g2_DrawStr(u8g2, 2, 28, buf);

	u8g2_SendBuffer(u8g2);
}


void display_task (void * pvParameters) {
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
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (CACHED_COUNTER != COUNTER) {
                printValue(&u8g2);
            }
        }
        vTaskDelete(NULL);
}
