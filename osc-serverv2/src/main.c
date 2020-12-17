#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "clocking.h"
#include "sequencer.h"
#include "mywifi.h"
#include "udp-server.h"

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Wifi Setup
    ESP_ERROR_CHECK(example_connect());

    // Clock task start
    xTaskCreate(clocking_task, "clock", 4096, NULL, 1, NULL);

    // Gate/sequencer task start
    xTaskCreate(sequencer_task, "sequencer", 4096, NULL, 1, NULL);

    // Start UDP server
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
}
