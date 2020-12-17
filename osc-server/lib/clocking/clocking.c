/* esp_timer (high resolution timer) example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_timer.h"
#include "esp_log.h"

#include "clocking.h"

static void periodic_timer_callback(void* arg);

static const char* TAG = "clocking";
int64_t  NEXT_BEAT_TIME; // Global for app beat sync

void clocking_task (void *pvParamenters) {
//TODO probably put this beat duration business in a seperate function
    // int beats_per_measure = 4;
    int subbeats_per_beat = 1;
    // int beat_denomination = 4;
    float beats_per_minute = 240.0;
    int seconds_per_minute = 60;
    long useconds_per_second = 1000000;
    // Setup Maths
    int64_t subbeat_duration_usec;
    // int64_t measure_duration_usec;
    subbeat_duration_usec = seconds_per_minute * useconds_per_second / beats_per_minute / subbeats_per_beat ;

    // measure_duration_usec = subbeat_duration_usec * beats_per_measure / subbeats_per_beat;
    NEXT_BEAT_TIME = (int64_t) subbeat_duration_usec;
    ESP_LOGI(TAG, "subbeat_duration_usec value: %lld \n", subbeat_duration_usec);

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
    
    // Clocking loop
    while (1) {
        int64_t current_time = esp_timer_get_time();
        // Not time for next beat yet
        if (NEXT_BEAT_TIME > current_time){
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // Next beat lambda has passed, update next beat timestamp
        else {
            NEXT_BEAT_TIME = NEXT_BEAT_TIME + subbeat_duration_usec;
        }
    }
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    vTaskDelete(NULL);
}

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
}

