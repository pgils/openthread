/*
 * Gpio.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: pollop
 */

#include "Gpio.h"

void Gpio::InitLeds()
{
    static const uint8_t ledList[LEDCOUNT] = LEDS_LIST;

    for (int i = 0; i < LEDCOUNT; i++)
    {
        nrf_gpio_cfg_output(ledList[i]);
        nrf_gpio_pin_write(ledList[i], 1);
    }
}

void Gpio::InitButton(nrfx_gpiote_evt_handler_t handler)
{
    ret_code_t err_code;

    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_out_config_t out_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    err_code = nrfx_gpiote_out_init(LED1_G, &out_config);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull                    = NRF_GPIO_PIN_PULLDOWN;

    err_code = nrfx_gpiote_in_init(BUTTON_1, &in_config, handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

void Gpio::ToggleLed1()
{
    nrf_gpio_pin_toggle(LED1_G);
}

void Gpio::SetRgbLed(uint32_t led)
{
    static const uint8_t rgbList[3] = {LED2_R, LED2_G, LED2_B};

    for (int i = 0; i < 3; i++)
    {
        nrf_gpio_pin_write(rgbList[i], (led != rgbList[i]));
    }
}
