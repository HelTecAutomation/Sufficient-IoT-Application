
#include <stdint.h>        /* C99 types */
#include <stdio.h>        /* printf fprintf */
#include <stdlib.h>        /* malloc free */
#include <unistd.h>        /* lseek, close */
#include <fcntl.h>        /* open */
#include <string.h>        /* memset */
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */
#define DEBUG_SPI 0
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#if DEBUG_SPI == 1
    #define DEBUG_MSG(str)                fprintf(stderr, str)
    #define DEBUG_PRINTF(fmt, args...)    fprintf(stderr,"%s:%d: "fmt, __FUNCTION__, __LINE__, args)
    #define CHECK_NULL(a)                if(a==NULL){fprintf(stderr,"%s:%d: ERROR: NULL POINTER AS ARGUMENT\n", __FUNCTION__, __LINE__);return SPI_ERROR;}
#else
    #define DEBUG_MSG(str)
    #define DEBUG_PRINTF(fmt, args...)
    #define CHECK_NULL(a)                if(a==NULL){return SPI_ERROR;}
#endif

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define READ_ACCESS     0x00
#define WRITE_ACCESS    0x80
#define SPI_SPEED       5000000
#define SPI_DEV_PATH    "/dev/spidev0.0"
/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

/* SPI initialization and configuration */
int spi_open(void **spi_target_ptr) {
    int *spi_device = NULL;
    int dev;
    int a=0, b=0;
    int i;

    /* check input variables */
    CHECK_NULL(spi_target_ptr); /* cannot be null, must point on a void pointer (*spi_target_ptr can be null) */

    /* allocate memory for the device descriptor */
    spi_device = malloc(sizeof(int));
    if (spi_device == NULL) {
        DEBUG_MSG("ERROR: MALLOC FAIL\n");
        return SPI_ERROR;
    }

    /* open SPI device */
    dev = open(SPI_DEV_PATH, O_RDWR);
    if (dev < 0) {
        DEBUG_PRINTF("ERROR: failed to open SPI device %s\n", SPI_DEV_PATH);
        return SPI_ERROR;
    }

    /* setting SPI mode to 'mode 0' */
    i = SPI_MODE_0;
    a = ioctl(dev, SPI_IOC_WR_MODE, &i);
    b = ioctl(dev, SPI_IOC_RD_MODE, &i);
    if ((a < 0) || (b < 0)) {
        DEBUG_MSG("ERROR: SPI PORT FAIL TO SET IN MODE 0\n");
        close(dev);
        free(spi_device);
        return SPI_ERROR;
    }

    /* setting SPI max clk (in Hz) */
    i = SPI_SPEED;
    a = ioctl(dev, SPI_IOC_WR_MAX_SPEED_HZ, &i);
    b = ioctl(dev, SPI_IOC_RD_MAX_SPEED_HZ, &i);
    if ((a < 0) || (b < 0)) {
        DEBUG_MSG("ERROR: SPI PORT FAIL TO SET MAX SPEED\n");
        close(dev);
        free(spi_device);
        return SPI_ERROR;
    }

    /* setting SPI to MSB first */
    i = 0;
    a = ioctl(dev, SPI_IOC_WR_LSB_FIRST, &i);
    b = ioctl(dev, SPI_IOC_RD_LSB_FIRST, &i);
    if ((a < 0) || (b < 0)) {
        DEBUG_MSG("ERROR: SPI PORT FAIL TO SET MSB FIRST\n");
        close(dev);
        free(spi_device);
        return SPI_ERROR;
    }

    /* setting SPI to 8 bits per word */
    i = 0;
    a = ioctl(dev, SPI_IOC_WR_BITS_PER_WORD, &i);
    b = ioctl(dev, SPI_IOC_RD_BITS_PER_WORD, &i);
    if ((a < 0) || (b < 0)) {
        DEBUG_MSG("ERROR: SPI PORT FAIL TO SET 8 BITS-PER-WORD\n");
        close(dev);
        return SPI_ERROR;
    }

    *spi_device = dev;
    *spi_target_ptr = (void *)spi_device;
    DEBUG_MSG("Note: SPI port opened and configured ok\n");
    return SPI_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* SPI release */
int spi_close(void *spi_target) {
    int spi_device;
    int a;

    /* check input variables */
    CHECK_NULL(spi_target);

    /* close file & deallocate file descriptor */
    spi_device = *(int *)spi_target; /* must check that spi_target is not null beforehand */
    a = close(spi_device);
    free(spi_target);

    /* determine return code */
    if (a < 0) {
        DEBUG_MSG("ERROR: SPI PORT FAILED TO CLOSE\n");
        return SPI_ERROR;
    } else {
        DEBUG_MSG("Note: SPI port closed\n");
        return SPI_SUCCESS;
    }
}


int spi_write(void *spi_target,  uint8_t *data,uint8_t size)
{
    int spi_device;
    uint8_t out_buf[256];
    uint8_t command_size;
    struct spi_ioc_transfer k;
    int a;

    /* check input variables */
    CHECK_NULL(spi_target);


    spi_device = *(int *)spi_target; /* must check that spi_target is not null beforehand */

    /* prepare frame to be sent */
    command_size  =size;
    for(int i = 0;i < command_size; i++)
    {
        out_buf[i]  = data[i];
    }
    /* I/O transaction */
    memset(&k, 0, sizeof(k)); /* clear k */
    k.tx_buf = (unsigned long) out_buf;
    k.len = command_size;
    k.speed_hz = SPI_SPEED;
    k.cs_change = 0;
    k.bits_per_word = 8;
    a = ioctl(spi_device, SPI_IOC_MESSAGE(1), &k);

    /* determine return code */
    if (a != (int)k.len) {
        DEBUG_MSG("ERROR: SPI WRITE FAILURE\n");
        return SPI_ERROR;
    } else {
        DEBUG_MSG("Note: SPI write success\n");
        return SPI_SUCCESS;
    }
}

/* Burst (multiple-byte) read */
int spi_read(void *spi_target,  uint8_t *write_data, uint16_t write_data_size,uint8_t *read_data ,uint8_t read_data_size) 
{
    int spi_device;
    uint8_t command[256];
    uint8_t command_size;
    struct spi_ioc_transfer k[2];
    int size_to_do, chunk_size, offset;
    int byte_transfered = 0;
    int i;

    /* check input parameters */
    CHECK_NULL(spi_target);
    CHECK_NULL(read_data);
    CHECK_NULL(write_data);  
    if (read_data_size == 0) {
        DEBUG_MSG("ERROR: BURST OF NULL LENGTH\n");
        return SPI_ERROR;
    }

    spi_device = *(int *)spi_target; /* must check that spi_target is not null beforehand */

    size_to_do = read_data_size;
    command_size  =write_data_size;
    for(int i = 0;i < command_size; i++)
    {
        command[i]  = write_data[i];
    }
    /* I/O transaction */
    memset(&k, 0, sizeof(k)); /* clear k */
    k[0].tx_buf = (unsigned long) &command[0];
    k[0].len = command_size;
    k[0].cs_change = 0;
    k[1].cs_change = 0;
    for (i=0; size_to_do > 0; ++i) {
        chunk_size = (size_to_do < BURST_CHUNK) ? size_to_do : BURST_CHUNK;
        offset = i * BURST_CHUNK;
        k[1].rx_buf = (unsigned long)(read_data + offset);
        k[1].len = chunk_size;
        byte_transfered += (ioctl(spi_device, SPI_IOC_MESSAGE(2), &k) - k[0].len );
        DEBUG_PRINTF("BURST READ: to trans %d # chunk %d # transferred %d \n", size_to_do, chunk_size, byte_transfered);
        size_to_do -= chunk_size;  /* subtract the quantity of data already transferred */
    }

    /* determine return code */
    if (byte_transfered != read_data_size) {
        DEBUG_MSG("ERROR: SPI BURST READ FAILURE\n");
        return SPI_ERROR;
    } else {
        DEBUG_MSG("Note: SPI burst read success\n");
        return SPI_SUCCESS;
    }
}



/* --- EOF ------------------------------------------------------------------ */
