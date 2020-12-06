#include <stdbool.h>
#include <driver/gpio.h>

// GATE output pins
#define GATEOUT_0 25
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GATEOUT_0)
// #define GATEout_1 26
// #define GATEout_2 4
// #define GATEout_3 27

esp_err_t gpio_config(const gpio_config_t *gpio_cfg);

void gate_out(void *parameters) {
  int GATE_VALUE;
  GATE_VALUE = (int)parameters;

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE; //disable interrupt
  io_conf.mode = GPIO_MODE_OUTPUT; //set as output mode
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; //bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pull_down_en = 0; //disable pull-down mode
  io_conf.pull_up_en = 0; //disable pull-up mode
  gpio_config(&io_conf); //configure GPIO with the given settings

  while(1) {
    gpio_set_level(GATEOUT_0, GATE_VALUE);
  }
}

