#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "data_conversion.h"
//Note: size must be an integer multiple of 2
int str_to_array(const char* str, uint8_t* arr, uint16_t size)
{
	uint8_t temp =0;
	uint8_t data =0;
	uint16_t count = 0;
	char *str_temp = (char *)str;
	if ((size % 2) != 0)
	{
		printf("config Parameter error, please enter a hexadecimal number\r\n");
		return -1;
	}
	while (count < size)
	{
		count++;
		switch (*str_temp++)
		{
		case '0':
			temp = 0x00;
			break;
		case '1':
			temp = 0x01;
			break;
		case '2':
			temp = 0x02;
			break;
		case '3':
			temp = 0x03;
			break;
		case '4':
			temp = 0x04;
			break;
		case '5':
			temp = 0x05;
			break;
		case '6':
			temp = 0x06;
			break;
		case '7':
			temp = 0x07;
			break;
		case '8':
			temp = 0x08;
			break;
		case '9':
			temp = 0x09;
			break;
		case 'a':
		case 'A':
			temp = 0x0a;
			break;
		case 'b':
		case 'B':
			temp = 0x0b;
			break;
		case 'c':
		case 'C':
			temp = 0x0c;
			break;
		case 'd':
		case 'D':
			temp = 0x0d;
			break;
		case 'e':
		case 'E':
			temp = 0x0e;
			break;
		case 'f':
		case 'F':
			temp = 0x0f;
			break;
		default:
			return -1;
		}
		if (count % 2 == 0)
		{
			arr[(count / 2) - 1] = data + temp;
		}
		else
		{
			data = temp << 4;
		}
	}
	return 0;
}

int str_to_int( const char* str, uint32_t *integer, uint8_t size)
{
	uint8_t temp = 0;
	uint32_t data = 0;
	uint16_t count = 0;
	char* str_temp = (char *)str;
	if ((size % 2) != 0)
	{
		printf("Parameter error, please enter a hexadecimal number\r\n");
		return -1;
	}
	while (count < size)
	{
		count++;
		switch (*str_temp++)
		{
		case '0':
			temp = 0x00;
			break;
		case '1':
			temp = 0x01;
			break;
		case '2':
			temp = 0x02;
			break;
		case '3':
			temp = 0x03;
			break;
		case '4':
			temp = 0x04;
			break;
		case '5':
			temp = 0x05;
			break;
		case '6':
			temp = 0x06;
			break;
		case '7':
			temp = 0x07;
			break;
		case '8':
			temp = 0x08;
			break;
		case '9':
			temp = 0x09;
			break;
		case 'a':
		case 'A':
			temp = 0x0a;
			break;
		case 'b':
		case 'B':
			temp = 0x0b;
			break;
		case 'c':
		case 'C':
			temp = 0x0c;
			break;
		case 'd':
		case 'D':
			temp = 0x0d;
			break;
		case 'e':
		case 'E':
			temp = 0x0e;
			break;
		case 'f':
		case 'F':
			temp = 0x0f;
			break;
		default:
			return -1;
		}

		data = (data << 4) + temp;
	}
	*integer = data;
	return 0;
}