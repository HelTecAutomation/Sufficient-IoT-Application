#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>        /* C99 types*/

// #include "config.h"    /* library configuration options (dynamically generated) */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

#define SPI_SUCCESS     0
#define SPI_ERROR       -1
#define BURST_CHUNK     256
/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

/**
@brief LoRa concentrator SPI setup (configure I/O and peripherals)
@param spi_target_ptr pointer on a generic pointer to SPI target (implementation dependant)
@return status of register operation (LGW_SPI_SUCCESS/LGW_SPI_ERROR)
*/

int spi_open(void **spi_target_ptr);

/**
@brief LoRa concentrator SPI close
@param spi_target generic pointer to SPI target (implementation dependant)
@return status of register operation (LGW_SPI_SUCCESS/LGW_SPI_ERROR)
*/

int spi_close(void *spi_target);




int spi_write(void *spi_target,  uint8_t *data,uint8_t size); 

int spi_read(void *spi_target,  uint8_t *write_data, uint16_t write_data_size,uint8_t *read_data ,uint8_t read_data_size);
#endif

/* --- EOF ------------------------------------------------------------------ */
