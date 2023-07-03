/*!
 * \file      gpio.c
 *
 * \brief     GPIO driver implementation
 *
 * \remark: Relies on the specific board GPIO implementation as well as on
 *          IO expander driver implementation if one is available on the target
 *          board.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "sx1262_gpio.h"
#include "gpio.h"
#include <stdio.h>
// #define printf printf
int ret = 0;
void GpioInit( Gpio_t *obj, uint8_t pin, uint8_t mode,  PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;
    if ((ret = unexport_gpio_pin(obj->pin)) == -1) {   
        printf("unexport_gpio_pin(%d) failed\n", obj->pin);
    }
   if ((ret = export_gpio_pin(obj->pin)) == -1) {   
        printf("export_gpio_pin(%d) failed\n", obj->pin);
    }
    if ((ret = set_gpio_direction(obj->pin, mode)) == -1) {
        printf("set_gpio_direction(%d) failed\n", obj->pin);
    }

    if ((ret = set_gpio_value(obj->pin, value)) == -1) {
        printf("set_gpio_value(%d) failed\n", obj->pin);
    }
}

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    if ((ret = set_gpio_edge(obj->pin, irqMode)) == -1) {
        printf("set_gpio_edge(%d) failed\n", obj->pin);
    }
    register_interupt_handle(obj->pin,irqHandler);
    //这里有个回调函数的功能没有实现
}

void GpioRemoveInterrupt( Gpio_t *obj )
{
    if ((ret = set_gpio_edge(obj->pin, NO_IRQ)) == -1) {
        printf("set_gpio_edge(%d) failed\n", obj->pin);
    }
}

void GpioWrite( Gpio_t *obj, uint32_t value )
{
   if ((ret = set_gpio_value(obj->pin, value)) == -1) {
        printf("set_gpio_value(%d) failed\n", obj->pin);
    }
}

void GpioToggle( Gpio_t *obj )
{
    uint8_t value;
    value = get_gpio_value(obj->pin);
    if(value ==1)
    {
	    if ((ret = set_gpio_value(obj->pin, 0)) == -1) {
        printf("set_gpio_value(%d) failed\n", obj->pin);
    	}
    }
    else
    {
	    if ((ret = set_gpio_value(obj->pin, 1)) == -1) {
       		 printf("set_gpio_value(%d) failed\n", obj->pin);
    	}
    }
}

uint32_t GpioRead( Gpio_t *obj )
{
    return get_gpio_value(obj->pin);
}
