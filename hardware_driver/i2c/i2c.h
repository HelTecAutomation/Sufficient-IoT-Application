#ifndef I2C_h
#define I2C_h

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_JOIN_ERR	0b00000001
#define I2C_WRITE_ERR	0b00000010
#define I2C_READ_ERR	0b00000100

struct i2c_t;		/*I2C opaque struct. Use the typedefed type i2c instead*/
typedef struct i2c_t * i2c;		/*I2C object*/

/*	i2c_open: i2c bus creation.
 *
 *	Arguments:
 *		filename: Name of the bus. eg: "/dev/i2c-0"
 *
 *	Returns:
 *		Success: A i2c object
 *		Failure: NULL. errno will be defined by malloc or pthread_mutex_init
 *
 *	Example:
 *
 *	#include "i2c.h"
 *
 *	int main(int argc, char *argv[])
 *	{
 *		i2c bus;
 *
 *		bus = i2c_open("/dev/i2c-2");
 *		...
 *	} */
i2c i2c_open(const char *filename) __attribute__((warn_unused_result));

/*	i2c_write: write a buffer to a desired address
 *
 *	Arguments:
 *		bus: i2c bus object.
 *		addr: device address.
 *		buffer: buffer to be written.
 *		buffersize: size of the buffer.
 *
 *	Returns:
 *		Success: 0.
 *		Failure: A value other than 0.
 *				I2C_JOIN_ERR if ioctl call to join i2c bus has failed.
 *				I2C_WRITE_ERR if the write call to i2c file descriptor has failed.
 *			errno will be set by the respective function.
 *
 *	Notes:	The referenced register of the i2c device should be placed as the first
 *			value of the buffer. This function aims a more flexible communication, but
 *			requires a buffer allocation. To avoid this, use i2c_reg_write instead.
 *
 *	Example: write 0, 1, 2, ..., to device with addr 0x50, register 0x1
 *
 *	#include "i2c.h"
 *
 *	#define DEVICE_ADDR	0x50
 *	#define DEVICE_REG	0x01
 *
 *	int main(int argc, char *argv[])
 *	{
 *		i2c bus;
 *		unsigned char buffer[] = {DEVICE_REG, 1, 2, ...};
 *
 *		bus = i2c_open("/dev/i2c-0");
 *
 *		i2c_write(bus, DEVICE_ADDR, buffer, sizeof(buffer));
 *	}*/
int i2c_write(i2c bus, int addr, unsigned char *buffer, int buffersize);

/*	i2c_reg_write: short hand for i2c_write function
 *
 *	Arguments:
 *		bus: i2c bus object.
 *		addr: device address.
 *		reg: device register.
 *		data: value to be sended.
 *
 *	Returns:
 *		See i2c_write returns.
 *
 *	Notes:	This functions write to only one register per call. For sequential writes,
 *			use i2c_write directly. */
int i2c_reg_write(i2c bus, int addr, int reg, int data);

/*	i2c_read: read from i2c bus.
 *
 *	Arguments:
 *		bus: i2c bus object.
 *		addr: device address.
 *		buffer: buffer where the read data will be written.
 *		buffersize: size to be read.
 *
 *	Returns:
 *		Success: 0.
 *		Failure: A value other than 0.
 *				I2C_JOIN_ERR if ioctl call to join i2c bus has failed.
 *				I2C_READ_ERR if the read call to i2c file descriptor has failed.
 *			errno wil be set by the respective function.
 *
 *	Notes:	Check the device documentation about the read process. Usually, you
 *	should issue a blank write (no value) on the desired register to read its value.
 *
 *	Example: Read n values from register 0x10 on a device with address 0x50
 *
 *	#include "i2c.h"
 *
 *	#define DEVICE_ADDR	0x50
 *	#define DEVICE_REG	0x10
 *
 *	int main(int argc, char *argv[])
 *	{
 *		i2c bus;
 *		unsigned char buffer[n];
 *
 *		buffer[0] = DEVICE_REG;
 *
 *		bus = i2c_open("/dev/i2c-1");
 *
 *		i2c_write(bus, DEVICE_ADDR, buffer, 1);
 *		i2c_read(bus, DEVICE_ADDR, buffer, n);
 *	}
 *	*/
int i2c_read(i2c bus, int addr, unsigned char *buffer, int buffersize);

/*	i2c_close: Close a i2c bus.
 *
 *	Arguments:
 *		bus: i2c bus object.
 *
 *	Returns: none.
 *
 *	Example:
 *
 *	#include "i2c.h"
 *
 *	int main(int argc, char *argc[])
 *	{
 *		i2c bus;
 *
 *		bus = i2c_open("/dev/i2c");
 *
 *		...
 *
 *		i2c_close(bus);
 *	}*/
void i2c_close(i2c bus);

#endif
