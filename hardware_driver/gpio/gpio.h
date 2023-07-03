#ifndef __GPIO_BOARD_H__
#define __GPIO_BOARD_H__

// Direction
#define GPIO_IN                 (1) 
#define GPIO_OUT                (2) 

// Value
#define GPIO_LOW                (0)
#define GPIO_HIGH               (1)


//edge none , rising , falling , both
#define EDGE_NONE          	(0)
#define EDGE_RISING          	(1)
#define EDGE_FALLING         	(2)
#define EDGE_BOTH         	(3)

//nanopi
#define GPIO_PIN(x)             (x)



#define GPIO_FILENAME_DEFINE(pin,field) char fileName[255] = {0}; \
        sprintf(fileName, "/sys/class/gpio/gpio%d/%s", pin, field);

typedef void ( *IrqHandler )( void );

int export_gpio_pin(int pin);
int unexport_gpio_pin(int pin);

// GPIO_LOW or GPIO_HIGH
int set_gpio_value(int pin, int value);
int get_gpio_value(int pin);

// GPIO_IN or GPIO_OUT
int set_gpio_direction(int pin, int direction);
int get_gpio_direction(int pin);

//edge none , rising , falling , both
int set_gpio_edge(int pin, int edge);
int get_gpio_edge(int pin);

void register_interupt_handle(int pin, IrqHandler dioIrq);
#endif
