#include <stdint.h>
#include <openthread/platform/switchtest.h>
#include <hal/nrf_gpio.h>

#define LED2_B         NRF_GPIO_PIN_MAP(0,12)

uint8_t otToggleLed(void)
{
  nrf_gpio_cfg_output(LED2_B);
  nrf_gpio_pin_toggle(LED2_B);

  return 0;
}
