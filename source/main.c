#include "gpio.h"
#include <stdio.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <pico/time.h>

#define LED_PIN 25
#define SWITCH_1_PIN 26
#define SWITCH_2_PIN 27

//debounce check
int get_switch_value(int pin)
{
    if(get_gpio_level(pin) == 0)
    {
        sleep_ms(20);
        if(get_gpio_level(pin) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    //led
    set_gpio_function(LED_PIN, FUNC_SIO);
    set_gpio_mode(LED_PIN, MODE_OUT);
    set_gpio_level(LED_PIN,0);

    //switch 1
    set_gpio_function(SWITCH_1_PIN, FUNC_SIO);
    set_gpio_mode(SWITCH_1_PIN, MODE_IN);
    set_gpio_pull(SWITCH_1_PIN, PULL_UP);

    //switch 2
    set_gpio_function(SWITCH_2_PIN, FUNC_SIO);
    set_gpio_mode(SWITCH_2_PIN, MODE_IN);
    set_gpio_pull(SWITCH_2_PIN, PULL_UP);

    uint64_t last_toggle = 0;

    //state duration in microseconds
    uint32_t state_duration_us = 0;
    while(1)
    {
        int switch_1 = get_switch_value(SWITCH_1_PIN);
        int switch_2 = get_switch_value(SWITCH_2_PIN);

        if(!switch_1 && !switch_2)
        {
            state_duration_us = 0;
        }
        else if(switch_1 && !switch_2)
        {
            state_duration_us = 500000;
        }
        else if(!switch_1 && switch_2)
        {
            state_duration_us = 250000;
        }
        else if(switch_1 && switch_2)
        {
            state_duration_us = 125000;
        }

        uint64_t now = time_us_64();

        if(state_duration_us > 0)
        {
            if(now - last_toggle > state_duration_us)
            {
                set_gpio_level(LED_PIN, !get_gpio_level(LED_PIN));
                last_toggle = now;
            }
        }
        else
        {
            set_gpio_level(LED_PIN, 0);
            last_toggle = now;
        }
    }
}