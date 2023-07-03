#include "i2c.h"

struct i2c_t {
	int fd;
	pthread_mutex_t mutex;
};

i2c i2c_open(const char *filename) {
	i2c bus = (i2c)malloc(sizeof(struct i2c_t));

	// Open I2C file
	if((bus->fd = open(filename, O_RDWR)) < 0) {
		free(bus);
		return NULL;
	}

	// create bus mutex
	if(pthread_mutex_init(&bus->mutex, NULL)) {
		close(bus->fd);
		free(bus);
		return NULL;
	}

	return bus;
}

int i2c_write(i2c bus, int addr, unsigned char *buffer, int buffersize) {
	/* Lock mutex */
	pthread_mutex_lock(&bus->mutex);

	/* Join I2C. */
	if(ioctl(bus->fd, I2C_SLAVE, addr) < 0)
		return -1;

	/* Write to device */
	if(write(bus->fd, buffer, buffersize) != buffersize)
		return -1;

	/* Unlock mutex */
	pthread_mutex_unlock(&bus->mutex);

	return 0;
}

int i2c_reg_write(i2c bus, int addr, int reg, int data) {
	unsigned char buffer[] = {reg, data};

	return i2c_write(bus, addr, buffer, 2);
}

int i2c_read(i2c bus, int addr, unsigned char *buffer, int buffersize) {
	/* Lock mutex */
	pthread_mutex_lock(&bus->mutex);

	/* Join I2C. */
	if(ioctl(bus->fd, I2C_SLAVE, addr) < 0)
		return -1;

	/* Read from device */
	if(read(bus->fd, buffer, buffersize) != buffersize)
		return -1;

	/* Unlock mutex */
	pthread_mutex_unlock(&bus->mutex);

	return 0;
}

void i2c_close(i2c bus) {
	pthread_mutex_destroy(&bus->mutex);
	close(bus->fd);
	free(bus);
}
