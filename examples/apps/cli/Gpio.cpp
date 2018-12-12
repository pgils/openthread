/*
 * Gpio.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: pollop
 */

#include "Gpio.h"

static const uint8_t ledList[LEDCOUNT] = LEDS_LIST;

void Gpio::InitLeds()
{
    for (int i=0; i<LEDCOUNT; i++)
    {
        nrf_gpio_cfg_output(ledList[i]);
        nrf_gpio_pin_write(ledList[i], 1);
    }
}

// TODO: interrupt
void Gpio::InitButton()
{
    nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP);
}

void Gpio::ToggleLed1()
{
    nrf_gpio_pin_toggle(LED1_G);
}

void Gpio::SetRgbLed(uint32_t led)
{
    for (int i=0; i<LEDCOUNT; i++)
    {
        nrf_gpio_pin_write(ledList[i], (led != ledList[i]));
    }
}
