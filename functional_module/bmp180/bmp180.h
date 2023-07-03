#ifndef _BMP180_H_
#define _BMP180_H_
 
#include <stdbool.h>
#include <stdint.h>

#define	BMP180_SlaveAddress 0xEE //   Define the slave address of the device in the IIC bus.
 
#define BMP_AC1_ADDR        0xAA //  Define the address of the calibration register.
#define BMP_AC2_ADDR        0xAC  
#define BMP_AC3_ADDR        0xAE  
#define BMP_AC4_ADDR        0xB0  
#define BMP_AC5_ADDR        0xB2  
#define BMP_AC6_ADDR        0xB4  
#define BMP_B1_ADDR         0xB6  
#define BMP_B2_ADDR         0xB8  
#define BMP_MB_ADDR         0xBA  
#define BMP_MC_ADDR         0xBC  
#define BMP_MD_ADDR         0xBE  
 
#define CONTROL_REG_ADDR    0xF4 // Define the address of the calibration register. Control register, set different values in this register to set different conversion time, and different values can confirm the conversion of atmospheric pressure or temperature.
#define BMP_COVERT_TEMP     0x2E // The conversion temperature is 4.5MS.
#define BMP_COVERT_PRES_0   0x34 // Convert atmospheric pressure 4.5ms.
#define BMP_COVERT_PRES_1   0x74 // Convert atmospheric pressure 7.5ms.
#define BMP_COVERT_PRES_2   0xB4 // Convert atmospheric pressure 13.5ms.
#define BMP_COVERT_PRES_3   0xF4 // Convert atmospheric pressure 25.5ms.
#define BMP_OUT_MSB			0xF6 // ADC output high 8 bits.
#define BMP_OUT_LSB			0xF7 // ADC output low 8 bits.
#define BMP_OUT_XLSB		0xF8 // When measuring 19 bits, the ADC outputs the lowest 3 bits.
 
#define OSS_TIME_MS	       26                 // time interval.
#define BMP_COVERT_PRES	   BMP_COVERT_PRES_3  // Atmospheric pressure conversion time.
#define OSS                3	              // Atmospheric pressure conversion time.
#define PRESSURE_OF_SEA	   101325.0f	     // Refer to sea level pressure.
 
int bmp180_read_temperature(uint16_t * temp);
int bmp180_read_pressure(uint32_t * pressure);
 
void bmp180_init(void);
void bmp180_convert(float *temperature, int32_t *pressure);
void bmp180_altitude( float *altitide);
void bmp180_deinit(void);

#endif


