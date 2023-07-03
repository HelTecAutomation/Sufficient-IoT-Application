#include "gpio.h"
#include <pthread.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/stat.h> 
#include <sys/ioctl.h> 
#include <linux/fs.h> 
#include <errno.h> 
#include <stdarg.h>

#define errlog(errmsg) do{\
							perror(errmsg);\
							printf("%s --> %s --> %d\n", __FILE__, __func__, __LINE__);\
							exit(1);\
						 }while(0)

static int write_value_to_file(char* fileName, char* buff);
static int write_int_value_to_file(char* fileName, int value);
static int read_value_from_file(char* fileName, char* buff, int len);
static int read_int_value_from_file(char* fileName);



int export_gpio_pin(int pin) 
{
    int gpio = pin;
    int ret = write_int_value_to_file("/sys/class/gpio/export", gpio);
    if (ret > 0)
    {
        return 0;
    }
    else 
    {
        return -1;
    }
}

int unexport_gpio_pin(int pin) 
{
    int gpio = pin;
    return write_int_value_to_file("/sys/class/gpio/unexport", gpio);
}

int get_gpio_value(int pin) 
{
    int gpio =pin;
    GPIO_FILENAME_DEFINE(gpio, "value")
    
    return read_int_value_from_file(fileName);
}

int set_gpio_value(int pin, int value) 
{
    int gpio = pin;
    GPIO_FILENAME_DEFINE(gpio, "value")
    
    return write_int_value_to_file(fileName, value);
}

int set_gpio_direction(int pin, int direction) 
{
    int gpio =pin;
    char directionStr[10];
    GPIO_FILENAME_DEFINE(gpio, "direction")
    
    if (direction == GPIO_IN) 
    {
        strcpy(directionStr, "in");
    } 
    else if (direction == GPIO_OUT) 
    {
        strcpy(directionStr, "out");
    } 
    else 
    {
        printf("direction must be 1 or 2,  1->in, 2->out");
        return -1;
    }
    return write_value_to_file(fileName, directionStr);
}

int get_gpio_direction(int pin) 
{
    char buff[255] = {0};
    int direction;
    int ret;
    int gpio =pin;  
    GPIO_FILENAME_DEFINE(gpio, "direction")
    
    ret = read_value_from_file(fileName, buff, sizeof(buff)-1);
    if (ret >= 0) 
    {
        if (strncasecmp(buff, "out", 3)==0) 
        {
            direction = GPIO_OUT;
        } 
        else if (strncasecmp(buff, "in", 2)==0) 
        {
            direction = GPIO_IN;
        } 
        else 
        {
            printf("direction wrong, must be in or out,  but %s", buff);
            return -1;
        }
        return direction;
    }
    return ret;
}



int get_gpio_edge(int pin) 
{
    char buff[255] = {0};
    int edge;
    int ret;
    int gpio =pin;  
    GPIO_FILENAME_DEFINE(gpio, "edge")
    
    ret = read_value_from_file(fileName, buff, sizeof(buff)-1);
    if (ret >= 0) 
    {
        if (strncasecmp(buff, "none", strlen("none"))==0) 
        {
            edge = EDGE_NONE;
        } 
        else if (strncasecmp(buff, "rising", strlen("rising"))==0) 
        {
            edge = EDGE_RISING;
        } 
        else if (strncasecmp(buff, "falling", strlen("falling"))==0)
        {
            edge = EDGE_FALLING;
        } else if (strncasecmp(buff, "both", strlen("both"))==0) 
        {
            edge = EDGE_BOTH;
        }
        else 
        {
            printf("edge wrong, must be in or out,  but %s", buff);
            return -1;
        }
        return edge;
    }
    return ret;
}

int set_gpio_edge(int pin, int edge) 
{
    int gpio =pin;
    char edgeStr[10];
    GPIO_FILENAME_DEFINE(gpio, "edge")
    switch(edge)
    {
        case EDGE_NONE:
            strcpy(edgeStr, "none");
            break;
        case EDGE_RISING:
            strcpy(edgeStr, "rising");
            break;
       case EDGE_FALLING:
            strcpy(edgeStr, "falling");
            break;
       case EDGE_BOTH:
            strcpy(edgeStr, "both");
            break;
        default:
            printf("edge must be 0,1 or 2,3  1->rising, 2->falling");
            return -1;
    }

    return write_value_to_file(fileName, edgeStr);
}

//GPIO interrupt processing related.
IrqHandler lora_dio_Irq;
int interput_pin;
/*Interrupt detection.*/
void  *Call_interrupt_handler(void *arg)
{
    int fd,epfd,res ,ret;
    int len;
    int buf[2] = {11};
    char fileName[255];
    struct epoll_event evt;
    struct epoll_event evts;
    sprintf(fileName, "/sys/class/gpio/gpio%d/value", interput_pin);
    fd= open(fileName, O_RDONLY |O_TRUNC  ,0777);

    epfd = epoll_create(1);
    if (epfd < 0) 
    {
        perror("epoll_create error");
    }
    evt.events = EPOLLIN |EPOLLET;    //LT horizontal trigger (default).
    evt.data.fd = fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD,fd, &evt);
    if (ret < 0) 
    {
        perror("epoll_ctl error");
    }

    while (1)
    {
        int buffers[1]  ={66};
        res = epoll_wait(epfd, &evts, 10, -1);
        if (res < 0) 
        {
            perror("epoll_wait error");
        }

        lseek(fd,0,SEEK_SET);
        ret = read(fd,buffers,1);
        if(ret < 0)
        {
            perror("read error");
        }
        if ((evts.data.fd == fd) && (buffers[0] =='1')) 
        {
            lora_dio_Irq(); 
            lseek(fd,0,SEEK_SET);
        }
    }
    close(fd);
    unexport_gpio_pin(interput_pin);
}


void register_interupt_handle(int pin, IrqHandler dioIrq)
{
    static uint8_t status = 0;
    int ret;
    pthread_t interupt_thread;
    interput_pin = pin;
    lora_dio_Irq = dioIrq;
    if(status ==0)
    {
        ret = pthread_create(&interupt_thread, NULL, Call_interrupt_handler, NULL);
        if(ret != 0)
        {
            printf("create interupt_thread failed \n");
        }
        status ++;
    }
}



static int write_value_to_file(char* fileName, char* buff) 
{
    int ret;
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL) 
    {
        printf("Unable to open file %s", fileName);
        ret = -1;
    } 
    else 
    {
        ret = fwrite(buff, strlen(buff), 1, fp);
        fclose(fp);
    }
    return ret;
}


static int write_int_value_to_file(char* fileName, int value) 
{
    char buff[50];
    sprintf(buff, "%d", value);
    return write_value_to_file(fileName, buff);
}


static int read_value_from_file(char* fileName, char* buff, int len) 
{
    int ret = -1;
    FILE *fp = fopen(fileName,"r");
    if (fp == NULL) 
    {
        printf("Unable to open file %s",fileName);
        return -1;
    } 
    else 
    {
        if (fread(buff, sizeof(char), len, fp)>0) 
        {
            ret = 0;
        }
    }
    fclose(fp);
    return ret;
}


static int read_int_value_from_file(char* fileName) 
{
    char buff[255];
    memset(buff, 0, sizeof(buff));
    int ret = read_value_from_file(fileName, buff, sizeof(buff)-1);
    if (ret == 0) 
    {
        return atoi(buff);
    }
    return ret;
}
