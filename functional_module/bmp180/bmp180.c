#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "bmp180.h"
#include "i2c.h"

#define errlog(errmsg) do{\
							perror(errmsg);\
							printf("%s --> %s --> %d\n", __FILE__, __func__, __LINE__);\
							exit(1);\
						 }while(0)

static i2c bmp180_i2c;
#define BMP180_I2C_ADDRESS 0X77
#define BMP180_I2C_BUS  "/dev/i2c-2"

 
// Define the variable to access the EEPROM calibration value.
static int16_t ac1;
static int16_t ac2; 
static int16_t ac3; 
static uint16_t ac4;
static uint16_t ac5;
static uint16_t ac6;
static int16_t b1; 
static int16_t b2;
static int16_t mb;
static int16_t mc;
static int16_t md;
 

static int bmp180_multiple_read(uint8_t reg_address, uint16_t *read_data)
{
    uint8_t reg_data[2];
    int ret;
	ret = i2c_write(bmp180_i2c, BMP180_I2C_ADDRESS, &reg_address,1);
	if(ret < 0 )
	{
		errlog("bmp180_multiple_read error");
	}

	ret = i2c_read(bmp180_i2c, BMP180_I2C_ADDRESS, reg_data, 2);
	if(ret < 0 )
	{
		errlog("bmp180_multiple_read error");
	}

    *read_data = ((uint16_t)reg_data[0])<<8 | (uint16_t)reg_data[1];
    return  ret ;
}
 
// Read the temperature value.
int bmp180_read_temperature(uint16_t * temp)
{
	int ret;
	ret = i2c_reg_write(bmp180_i2c,BMP180_I2C_ADDRESS,CONTROL_REG_ADDR,BMP_COVERT_TEMP);
	if(ret < 0 )
	{
		errlog("bmp180_read_temperature error");
	}
 
    usleep(5000);//delay 5 ms
    return bmp180_multiple_read(BMP_OUT_MSB, temp);
}
 
// Read the air pressure value.
int bmp180_read_pressure(uint32_t * pressure)
{
    int ret;
    uint8_t reg_data[3];
	uint8_t reg_address = BMP_OUT_MSB;
 	ret = i2c_reg_write(bmp180_i2c,BMP180_I2C_ADDRESS,CONTROL_REG_ADDR,BMP_COVERT_PRES);
	if(ret < 0 )
	{
		errlog("bmp180_read_pressure error");
	}
    usleep(OSS_TIME_MS*1000);
	
    // After the measurement is completed, read the value measured by the sensor.
	ret = i2c_write(bmp180_i2c, BMP180_I2C_ADDRESS, &reg_address,1);
	if(ret < 0 )
	{
		errlog("bmp180_read_pressure error");
	}
	ret = i2c_read(bmp180_i2c, BMP180_I2C_ADDRESS, reg_data, 3);
	if(ret < 0 )
	{
		errlog("bmp180_read_pressure error");
	}
	
    *pressure = (((uint32_t)reg_data[0]) << 16 | ((uint32_t)reg_data[1]) <<8 | (uint32_t)reg_data[2]) >> (8 - OSS);
	
    return ret;
}
 
// The sensor initializes and reads the EEPROM value.
void bmp180_init(void)
{
	bmp180_i2c = i2c_open(BMP180_I2C_BUS);
    bmp180_multiple_read( BMP_AC1_ADDR, (uint16_t *)&ac1 );
    bmp180_multiple_read( BMP_AC2_ADDR, (uint16_t *)&ac2 );
    bmp180_multiple_read( BMP_AC3_ADDR, (uint16_t *)&ac3 );
    bmp180_multiple_read( BMP_AC4_ADDR, (uint16_t *)&ac4 );
    bmp180_multiple_read( BMP_AC5_ADDR, (uint16_t *)&ac5 );
    bmp180_multiple_read( BMP_AC6_ADDR, (uint16_t *)&ac6 );
    bmp180_multiple_read( BMP_B1_ADDR,  (uint16_t *)&b1  );
    bmp180_multiple_read( BMP_B2_ADDR,  (uint16_t *)&b2  );
    bmp180_multiple_read( BMP_MB_ADDR,  (uint16_t *)&mb  );
    bmp180_multiple_read( BMP_MC_ADDR,  (uint16_t *)&mc  );
    bmp180_multiple_read( BMP_MD_ADDR,  (uint16_t *)&md  );
}
 
 
// Calibrate temperature and air pressure.
void bmp180_convert(float *temperature, int32_t *pressure)
{
    uint16_t ut;
    uint32_t up;
 
    int temp_state , pressure_state;
    int32_t x1, x2, b5, b6, x3, b3, p;
    uint32_t b4, b7;
 
	
    temp_state = bmp180_read_temperature(&ut);
    pressure_state = bmp180_read_pressure(&up);
	
    if(temp_state == 0 && pressure_state == 0)
    {
	// Calculate the temperature.
	x1 = ((int32_t)ut - ac6) * ac5 >> 15;
	x2 = ((int32_t) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	*temperature = ((b5 + 8) >> 4) * 0.1f;
 
	// Calculate the air pressure.
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = ((((int32_t)ac1 * 4 + x3) << OSS) + 2) >> 2;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * (b6 * b6>> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	{
	    p = (b7 * 2) / b4 ;
	}
	else  
	{
	    p = (b7 / b4) * 2;
	}
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	* pressure = p + ((x1 + x2 + 3791) >> 4);		
    }
}
 
// Calculate the altitude.
void bmp180_altitude( float *altitide)
{
	float temperature;
	int32_t pressure;
    bmp180_convert(&temperature, &pressure); // Calculate the pressure.
    *altitide = 44330*(1 - pow((double)(pressure)/ PRESSURE_OF_SEA, 1.0f / 5.255f));
}
 
void bmp180_deinit(void)
{
	i2c_close(bmp180_i2c);
}