#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "hdc1080.h"
#include "i2c.h"

#define errlog(errmsg) do{\
							perror(errmsg);\
							printf("%s --> %s --> %d\n", __FILE__, __func__, __LINE__);\
							exit(1);\
						 }while(0)

static i2c hdc1080_i2c;
#define HDC1080_I2C_ADDRESS 0X40
#define HDC1080_I2C_BUS  "/dev/i2c-2"
void hdc1080_init(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature)
{
	int ret;
	uint8_t hdc1080_reg;
	uint16_t config_reg_value=0x0000;
	uint8_t config_arr[3] ={0x00};

	if (temperature == HDC1080_RESOLUTION_11BIT)
	{
		config_reg_value |= (1<<10); //11 bit
	}

	switch(humidity)
	{
		case HDC1080_RESOLUTION_8BIT:
			config_reg_value|= (1<<9);
			break;
		case HDC1080_RESOLUTION_11BIT:
			config_reg_value|= (1<<8);
			break;
	}
	config_arr[0]= HDC1080_CONFIGURATION;
	config_arr[1]= 0;
	config_arr[2]= (uint8_t)(config_reg_value&0x00ff);

	hdc1080_i2c = i2c_open(HDC1080_I2C_BUS);
	if(hdc1080_i2c ==NULL)
	{
		errlog("i2c_open error");
		return ;
	}

	ret =i2c_write(hdc1080_i2c, HDC1080_I2C_ADDRESS, (uint8_t *)config_arr,3);
	if(ret < 0)
	{
		errlog("i2c_write error");
		return ;
	}
}


void hdc1080_read_manufacturer_id(uint16_t *id)
{
	int ret;
	uint8_t data[2];
	uint8_t reg = HDC1080_MANUFACTURER_ID;

	ret = i2c_write(hdc1080_i2c, HDC1080_I2C_ADDRESS, &reg,1);
	if(ret < 0)
	{
		errlog("i2c_write error");
		return ;
	}
	ret = i2c_read(hdc1080_i2c, HDC1080_I2C_ADDRESS, data, 2);
	if(ret < 0)	
	{
		errlog("i2c_read  error");
		return ;
	}
	*id = (data[0] << 8 | data[1]);
}   


void hdc1080_read_temperature(double *temperature) 
{
	int ret;
	uint16_t temp =0;
	uint8_t reg = HDC1080_TEMPERATURE;
	uint8_t data[2] ={0};
	ret = i2c_write(hdc1080_i2c, HDC1080_I2C_ADDRESS, &reg,1);
	if(ret < 0)
	{
		errlog("i2c_write error");
		return ;
	}
	usleep(15000); //delay 15 ms
	ret = i2c_read(hdc1080_i2c, HDC1080_I2C_ADDRESS, data, 2);
	if(ret < 0)	
	{
		errlog("i2c_read  error");
		return ;
	}
 	temp = (data[0] << 8 | data[1]);
 	*temperature =  (temp / pow(2, 16)) * 165.0 - 40.0;
}

void hdc1080_read_humidity(double *humidity) 
{
	int ret;
	uint16_t temp =0;
	uint8_t reg = HDC1080_HUMIDITY;
	uint8_t data[2] ={0};

	ret = i2c_write(hdc1080_i2c, HDC1080_I2C_ADDRESS, &reg,1);
	if(ret < 0)
	{
		errlog("i2c_write error");
		return ;
	}
	usleep(15000); //delay 15 ms
	ret = i2c_read(hdc1080_i2c, HDC1080_I2C_ADDRESS, data, 2);
	if(ret < 0)
	{
		errlog("i2c_read  error");
		return ;
	}

 	temp = (data[0] << 8 | data[1]);
 	*humidity =  (temp / pow(2, 16)) * 100.0;
}

void hdc1080_deinit(void)
{
	i2c_close(hdc1080_i2c);
}