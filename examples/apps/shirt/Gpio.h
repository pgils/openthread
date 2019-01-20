/*
 * Gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: pollop
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <hal/nrf_gpio.h>
#include <nrfx_gpiote.h>

#define LED1_G      NRF_GPIO_PIN_MAP(0, 6)
#define LED2_R      NRF_GPIO_PIN_MAP(0, 8)
#define LED2_G      NRF_GPIO_PIN_MAP(1, 9)
#define LED2_B      NRF_GPIO_PIN_MAP(0, 12)
#define LED2_OFF    -1
#define BUTTON_1    NRF_GPIO_PIN_MAP(1, 6)

#define LEDS_LIST                      \
    {                                  \
        LED1_G, LED2_R, LED2_G, LED2_B \
    }
#define RGB_LIST               \
    {                          \
        LED2_G, LED2_R, LED2_B \
    }
#define LEDCOUNT 4

class Gpio
{
public:
    // This function initialises LD1 and LD2
    static void InitLeds();

    // This function initialises SW1
    static void InitButton(nrfx_gpiote_evt_handler_t handler);

    // This function sets the output color of LD2
    // @param[in] led       Pin to set (one of RGB_LIST) or LED2_OFF
    static void SetRgbLed(uint32_t led);

    // This function toggles LD1
    static void SetLed1(bool on = false);
};

#endif /* GPIO_H_ */
