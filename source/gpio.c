#include "gpio.h"

//Base Registers used for GPIO configuration
static volatile uint32_t* const io_bank0_base = (volatile uint32_t*)0x40014000;   // Base pointer for the IO_BANK0 registers
static volatile uint32_t* const pads_bank0_base = (volatile uint32_t*)0x4001C000; // Base pointer for the PADS_BANK0 registers
static volatile uint32_t* const sio_base = (volatile uint32_t*)0xD0000000;        // Base pointer for the SIO registers

//Configures the pin to be in the given mode using IO_BANK0
void set_gpio_function(const int pin, const int function)
{
    if(pin < 0 || pin > 29)
    {
        return;
    }
    if(function < 0 || function > 31)
    {
        return;
    }

    //___find the specific pins index inside io_bank0 and set the function
    uint32_t array_index = ((pin * 8) + 0x4) / 4;
    io_bank0_base[array_index] = function;
}

//Sets the input/output mode of the given pin using the SIO
//The input enable bit in the corresponding PADS_BANK0 register is set
void set_gpio_mode(const int pin, const int mode)
{
    if(pin < 0 || pin > 29)
    {
        return;
    }
    if(mode != MODE_IN && mode != MODE_OUT)
    {
        return;
    }
    //___create a sio mask where the pin is 1 and everything else is 0
    uint32_t sio_register_set_mask = (1 << pin);

    if(mode == MODE_OUT)
    {
        //___9 index is GPIO_OE_SET from datasheet 0x024 divided by 4
        sio_base[9] = sio_register_set_mask;
    }
    else
    {
        //___10 index is GPIO_OE_CLR from datasheet 0x028 divided by 4
        sio_base[10] = sio_register_set_mask;
    }


    //___find the specific pins index inside pads_bank0 array
    uint32_t array_index = ((pin * 4) + 0x4) / 4;

    //___create a pads pin mask for input enable - bit 6 in pads_bank0 register and set it
    uint32_t pads_ie_bit_mask = (1 << 6);
    pads_bank0_base[array_index] |= pads_ie_bit_mask;
}

//Sets the output level of the given pin using the SIO
void set_gpio_level(const int pin, const int level)
{
    if(pin < 0 || pin > 29)
    {
        return;
    }

    uint32_t sio_register_set_mask = (1 << pin);
    if(level == 1)
    {
        //___5 index is GPIO_OUT_SET from datasheet 0x014 divided by 4
        sio_base[5] = sio_register_set_mask;
    }
    else
    {
        //___6 index is GPIO_OUT_CLR from datasheet 0x018 divided by 4
        sio_base[6] = sio_register_set_mask;
    }
}

//Returns the input level of the given pin using the SIO
int get_gpio_level(const int pin)
{
    if(pin < 0 || pin > 29)
    {
        return -1;
    }

    //___1 index is GPIO_IN from datasheet 0x004 divided by 4
    uint32_t gpio_in_states = sio_base[1];

    //___isolate pins value
    uint32_t pin_value = gpio_in_states & (1 << pin);

    if(pin_value == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//Sets the pull of the given pin using the PADS_BANK0 registers
void set_gpio_pull(const int pin, const int pull)
{
    if(pin < 0 || pin > 29)
    {
        return;
    }

    int array_index = ((pin * 4) + 0x4) / 4;

    //___pull up enable - bit 3 in pads_bank0
    uint32_t pull_up_enable_mask = (1 << 3);
    //___pull down enable - bit 2 in pads_bank0
    uint32_t pull_down_enable_mask = (1 << 2);

    uint32_t current_configuration = pads_bank0_base[array_index];
    uint32_t new_configuration;
    if(pull == PULL_UP)
    {
        new_configuration = current_configuration | pull_up_enable_mask;
        new_configuration &= (~pull_down_enable_mask);
    }
    else if(pull == PULL_DOWN)
    {
        new_configuration = current_configuration | pull_down_enable_mask;
        new_configuration &= (~pull_up_enable_mask);
    }
    else if(pull == PULL_NONE)
    {
        new_configuration = current_configuration & (~pull_down_enable_mask);
        new_configuration = new_configuration & (~pull_up_enable_mask);
    }
    else
    {
        return;
    }
    pads_bank0_base[array_index] = new_configuration;
}