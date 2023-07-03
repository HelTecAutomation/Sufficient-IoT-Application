/*!
 * \file      sx1262dvk1cas-board.c
 *
 * \brief     Target board SX1262DVK1CAS shield driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <stdlib.h>
#include "sx126x-board.h"
#include "sx126x.h"
// #include "Arduino.h"
#include "board-config.h"
#include <stdio.h>
#include "time/delay.h"
#include "spi.h"

//spi句柄
void *spi_target = NULL;
uint8_t gPaOptSetting = 0;
#define OUTPUT 2
#define INPUT 1
// extern uint8_t SpiInOut(Spi_t *obj, uint8_t outData );
SX126x_t SX126x;
uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xIoInit( void )
{
    GpioInit( &SX126x.Reset, LORA_RESET, OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    GpioInit( &SX126x.BUSY, LORA_BUSY, INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX126x.DIO1, LORA_DIO_1, INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
//	attachInterrupt(RADIO_DIO_1,dioIrq,RISING);
    GpioSetInterrupt( &SX126x.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, dioIrq );
}

void SX126xReset( void )
{
	//pinMode(SX126x.Reset.pin,OUTPUT);
	DelayMs( 20 );
	GpioWrite(&SX126x.Reset,1);
	DelayMs( 50 );
	GpioWrite(&SX126x.Reset,0);
	DelayMs( 50 );
	GpioWrite(&SX126x.Reset,1);
	DelayMs( 50 );

    /*
    GpioInit( &SX126x.Reset, LORA_RESET, OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
    DelayMs( 40 );
    GpioInit( &SX126x.Reset, LORA_RESET, OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 ); // internal pull-up
    DelayMs( 40 );
    GpioInit( &SX126x.Reset, LORA_RESET, OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
    */
}

void SX126xWaitOnBusy( void )
{
	uint32_t timeout=0;
    while( GpioRead( &SX126x.BUSY ) == 1 )
    {
    	timeout++;
    	if(timeout>1000000)
    	{
    		printf("spi timeout\r\n");
    		break;
    	}
    	DelayMs(1);
    	//printf("Wait");
    }
}

int SX126xSpiInit( void )
{
    int ret;
    ret =spi_open(&spi_target);
    return ret;

}

void SX126xWakeup( void )
{
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    // SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
    // SpiInOut( &SX126x.Spi, 0x00 );

    uint8_t data[3] ={0};
    data[0] = RADIO_GET_STATUS;
    data[1] = 0;

    // SX126xWaitOnBusy( );

    spi_write(spi_target,  data, 2);
    // GpioWrite( &SX126x.Spi.Nss, 1 );;
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );
}


void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );
    uint8_t data[12] ={0};
    data[0] = ( uint8_t )command;
    for(int i = 0; i<size ; i++ )
    {
        data[1+i] = buffer[i];
    }  
    SX126xWaitOnBusy( );

    spi_write(spi_target,  data, size+1);

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

void SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{

    SX126xCheckDeviceReady( );
    uint8_t data[3] ={0};
    data[0] = ( uint8_t )command;
    data[1] = 0;

    SX126xWaitOnBusy( );
    spi_read(spi_target,  data, 2,buffer ,size);
    SX126xWaitOnBusy( );;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );
    uint8_t data[100] ={0};
    data[0] = RADIO_WRITE_REGISTER;
    data[1] = (uint8_t)(( address & 0xFF00 ) >> 8);
    data[2] = (uint8_t) (address & 0x00FF);
    if(size > 90)
    {
        printf("X126xWriteRegisters error\n");
    }

    for(int i = 0; i<size ; i++ )
    {
        data[3+i] = buffer[i];
    }  
    SX126xWaitOnBusy( );

    spi_write(spi_target,  data, size+3);

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    SX126xWaitOnBusy( );
    uint8_t data[5] ={0};
    data[0] = RADIO_READ_REGISTER;
    data[1] = (uint8_t)(( address & 0xFF00 ) >> 8);
    data[2] = (uint8_t) (address & 0x00FF);
    data[3] = 0;

    spi_read(spi_target,  data, 4,buffer ,size);
    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{

    SX126xCheckDeviceReady( );

    uint8_t data[256] ={0};
    data[0] = RADIO_WRITE_BUFFER;
    data[1] = offset;
    for(int i = 0; i<size ; i++ )
    {
        data[2+i] = buffer[i];
    }  
    SX126xWaitOnBusy( );

    spi_write(spi_target,  data, size+2);

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    uint8_t data[4] ={0};
    data[0] = RADIO_READ_BUFFER;
    data[1] = offset;
    data[2] = 0;
    SX126xWaitOnBusy( );
    spi_read(spi_target,  data, 3,buffer ,size);
    SX126xWaitOnBusy( );

}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetPaSelect( uint32_t channel )
{
    return SX1262;
}

void SX126xAntSwOn( void )
{
}

void SX126xAntSwOff( void )
{
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

uint8_t SX126xGetPaOpt( )
{
    return gPaOptSetting;
}

void SX126xSetPaOpt( uint8_t opt )
{
    if(opt>3) return;
    
    gPaOptSetting = opt;
}
