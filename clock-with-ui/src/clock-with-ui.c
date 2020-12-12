#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_log.h>
#include "driver/gpio.h"
#include "u8g2_esp32_hal.h"
#include <rotary_encoder.h>
#include "esp_timer.h"

//gatout_config configuration
#define GPIO_OUTPUT_IO_0   25
#define GPIO_OUTPUT_IO_1   26
#define GPIO_OUTPUT_IO_2   4
#define GPIO_OUTPUT_IO_3   27
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2)| (1ULL<<GPIO_OUTPUT_IO_3))
#define ESP_INTR_FLAG_DEFAULT 0

//display_config configuration
#define PIN_SDA 21
#define PIN_SCL 22

//encoder_config configuration
#define ROT_ENC_A_GPIO 5
#define ROT_ENC_B_GPIO 18
#define ENC_SW  19
#define ENABLE_HALF_STEPS false  // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT          100      // Set to a posi non-zero num to reset the position if this value is exceeded
#define FLIP_DIRECTION    false  // Set to true to reverse the clockwise/counterclockwise sense

int ROTARY_VALUE = 9;

static const char *TAG = "encoder";

void gateout_config (void) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT; //set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0; //disable pull-down mode
    io_conf.pull_up_en = 0; //disable pull-up mode
    gpio_config(&io_conf); //configure GPIO with the given settings
}

void printValue(u8g2_t *u8g2) {
 	char buf[256];

 	u8g2_ClearBuffer(u8g2);
	//u8g2_SetFont(u8g2,u8g2_font_ncenB14_tr);
	u8g2_SetFont(u8g2,u8g2_font_inr27_mn);
        sprintf(buf, "%d", (int) ROTARY_VALUE); 
	u8g2_DrawStr(u8g2,2,28,buf);

	u8g2_SendBuffer(u8g2);
}


void display_task (void *pvParameters) {
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = PIN_SDA;
	u8g2_esp32_hal.scl  = PIN_SCL;
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

        uint32_t loop = 0;
        
        while (1) {
            printValue(&u8g2);
            vTaskDelay(100 / portTICK_RATE_MS);
            loop++;
        }
        vTaskDelete(NULL);
}

static void timer_callback()
{
    long int tempo;
    tempo = ROTARY_VALUE / 60 / 1000000;
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "oneshot"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
    /* The timer has been created but is not running yet */
    ESP_ERROR_CHECK(esp_timer_start_once(timer, tempo));


    gpio_set_level(GPIO_OUTPUT_IO_3, 1);
    gpio_set_level(GPIO_OUTPUT_IO_3, 0);
}


void start_clock() {
    long int tempo;
    tempo = ROTARY_VALUE / 60 / 1000000;
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "oneshot"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
    /* The timer has been created but is not running yet */
    ESP_ERROR_CHECK(esp_timer_start_once(timer, tempo));
}


void app_main(void)
{
    gateout_config();
    start_clock();
    //encoder_config();
    const int SIZE = 16;
    int kick[16] = { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 };
    int snare[16] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    int hat[16] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };

    xTaskCreate(display_task, "displaying", 4096, NULL, 5, NULL);

    // esp32-rotary-encoder requires that the GPIO ISR service is installed before calling rotary_encoder_register()
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Initialise the rotary encoder device with the GPIOs for A and B signals
    rotary_encoder_info_t info = { 0 };
    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif

    // Create a queue for events from the rotary encoder driver.
    // Tasks can read from this queue to receive up to date position information.
    QueueHandle_t event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));

    while(1) {
        for (int i = 0; i < SIZE; i++) {
        vTaskDelay(500 / portTICK_RATE_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, kick[i]);
        gpio_set_level(GPIO_OUTPUT_IO_1, snare[i]);
        gpio_set_level(GPIO_OUTPUT_IO_2, hat[i]);
       
       // Wait for incoming events on the event queue.
        rotary_encoder_event_t event = { 0 };
        if (xQueueReceive(event_queue, &event, 1000 / portTICK_PERIOD_MS) == pdTRUE)
        {
            ESP_LOGI(TAG, "Event: posi %d, dir %s", event.state.position,
                event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
            ROTARY_VALUE = event.state.position;
        }
        else
        {
            // Poll current position and direction
            rotary_encoder_state_t state = { 0 };
            ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));
            ESP_LOGI(TAG, "Poll: position %d, direction %s", state.position,
                state.direction ? (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");

            ROTARY_VALUE = state.position;

            // Reset the device
            if (RESET_AT && (state.position >= RESET_AT || state.position <= -RESET_AT))
            {
                ESP_LOGI(TAG, "Reset");
                ESP_ERROR_CHECK(rotary_encoder_reset(&info));
            }
        }
    }
}
    ESP_ERROR_CHECK(rotary_encoder_uninit(&info));


}
