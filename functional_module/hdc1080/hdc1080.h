#ifndef _HDC1080_h
#define _HDC1080_h
#include <stdint.h>
typedef enum {
	HDC1080_RESOLUTION_8BIT,
	HDC1080_RESOLUTION_11BIT,
	HDC1080_RESOLUTION_14BIT,
} HDC1080_MeasurementResolution;

typedef enum {
	HDC1080_TEMPERATURE		= 0x00,
	HDC1080_HUMIDITY		= 0x01,
	HDC1080_CONFIGURATION	= 0x02,
	HDC1080_MANUFACTURER_ID = 0xFE,
	HDC1080_DEVICE_ID		= 0xFF,
	HDC1080_SERIAL_ID_FIRST	= 0xFB,
	HDC1080_SERIAL_ID_MID	= 0xFC,
	HDC1080_SERIAL_ID_LAST	= 0xFD,
} HDC1080_Pointers;


void hdc1080_read_manufacturer_id(uint16_t *id);
void hdc1080_read_temperature(double *temperature);
void hdc1080_init(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature);
void hdc1080_read_humidity(double *humidity);
void hdc1080_deinit(void);


#endif

