#pragma once
#include <stdint.h>

//___GPIO functions
//Input/Output
#define FUNC_SIO 5
//Disabled
#define FUNC_NONE 31

//___GPIO input/output modes
#define MODE_IN 0
#define MODE_OUT 1

//___Pull up/down modes
#define PULL_NONE 0
#define PULL_DOWN 1
#define PULL_UP 2

//Configures the pin to be in the given mode using IO_BANK0
void set_gpio_function(int pin, int function);

//Sets the input/output mode of the given pin using the SIO
//The input enable bit in the corresponding PADS_BANK0 register is set
void set_gpio_mode(int pin, int mode);

//Sets the output level of the given pin using the SIO
void set_gpio_level(int pin, int level);

//Returns the input level of the given pin using the SIO
int get_gpio_level(int pin);

//Sets the pull of the given pin using the PADS_BANK0 registers
void set_gpio_pull(int pin, int pull);