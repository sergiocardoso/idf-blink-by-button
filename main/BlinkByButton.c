#include <stddef.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_LED 2
#define GPIO_LED_PIN_SEL (1ULL << GPIO_LED)
#define GPIO_BUTTON 5
#define GPIO_BUTTON_PIN_SEL (1ULL << GPIO_BUTTON)
#define ESP_INTR_FLAG_DEFAULT 0

static TickType_t next = 0;
static bool led_state = false;

static void IRAM_ATTR button_handler(void *arg)
{
  TickType_t now = xTaskGetTickCountFromISR();
  if (now > next)
  {
    led_state = !led_state;
    gpio_set_level(GPIO_LED, led_state);
    next = now + 500 / portTICK_PERIOD_MS;
  }
};

static void init_hw(void)
{
  gpio_config_t io;

  io.mode = GPIO_MODE_OUTPUT;
  io.pin_bit_mask = GPIO_LED_PIN_SEL;
  io.intr_type = GPIO_INTR_DISABLE;
  io.pull_down_en = 0;
  io.pull_up_en = 0;

  gpio_config(&io);

  io.mode = GPIO_MODE_INPUT;
  io.pin_bit_mask = GPIO_BUTTON_PIN_SEL;
  io.intr_type = GPIO_INTR_NEGEDGE;
  io.pull_up_en = 1;

  gpio_config(&io);

  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add(GPIO_BUTTON, button_handler, NULL);
}

void app_main()
{
  init_hw();
  vTaskSuspend(NULL);
}