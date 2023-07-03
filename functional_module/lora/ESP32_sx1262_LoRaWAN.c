#include "ESP32_sx1262_LoRaWAN.h"
#include <stdlib.h>
#include <stdint.h>
#if(LoraWan_RGB==1)
#include "CubeCell_NeoPixel.h"
CubeCell_NeoPixel pixels(1, RGB, NEO_GRB + NEO_KHZ800);
#endif

#if defined( REGION_EU868 )
#include "loramac/region/RegionEU868.h"
#elif defined( REGION_EU433 )
#include "loramac/region/RegionEU433.h"
#elif defined( REGION_KR920 )
#include "loramac/region/RegionKR920.h"
#elif defined( REGION_AS923) || defined( REGION_AS923_AS1) || defined( REGION_AS923_AS2)
#include "loramac/region/RegionAS923.h"
#endif



/*loraWan default Dr when adr disabled*/
#ifdef REGION_US915
int8_t defaultDrForNoAdr = 3;
#else
int8_t defaultDrForNoAdr = 5;
#endif

/*AT mode, auto into low power mode*/
bool autoLPM = true;

/*loraWan current Dr when adr disabled*/
int8_t currentDrForNoAdr;

/*!
 * User application data size
 */
uint8_t appDataSize = 4;

/*!
 * User application data
 */
uint8_t appData[LORAWAN_APP_DATA_MAX_SIZE];


/*!
 * Defines the application data transmission duty cycle
 */
uint32_t txDutyCycleTime ;

/*!
 * Timer to handle the application data transmission duty cycle
 */
TimerEvent_t TxNextPacketTimer;

/*!
 * PassthroughMode mode enable/disable. don't modify it here. 
 * when use PassthroughMode, set it true in app.ino , Reference the example PassthroughMode.ino 
 */
bool passthroughMode = false;

/*!
 * when use PassthroughMode, Mode_LoraWan to set use lora or lorawan mode . don't modify it here. 
 * it is used to set mode lora/lorawan in PassthroughMode.
 */
bool modeLoraWan = true;

/*!
 * Indicates if a new packet can be sent
 */
static bool nextTx = true;




/*!
 * \brief   Prepares the payload of the frame
 *
 * \retval  [0: frame could be send, 1: error]
 */
bool SendFrame( void )
{
	lwan_dev_params_update();
	
	McpsReq_t mcpsReq;
	LoRaMacTxInfo_t txInfo;
	// LORAWANLOG;
	if( LoRaMacQueryTxPossible( appDataSize, &txInfo ) != LORAMAC_STATUS_OK )
	{
		// Send empty frame in order to flush MAC commands
		printf("payload length error ...\r\n");
		mcpsReq.Type = MCPS_UNCONFIRMED;
		mcpsReq.Req.Unconfirmed.fBuffer = NULL;
		mcpsReq.Req.Unconfirmed.fBufferSize = 0;
		mcpsReq.Req.Unconfirmed.Datarate = currentDrForNoAdr;
		//return false;
	}
	else
	{
		if( isTxConfirmed == false )
		{
			BASE_PRINTF("unconfirmed uplink sending ...\r\n");
			mcpsReq.Type = MCPS_UNCONFIRMED;
			mcpsReq.Req.Unconfirmed.fPort = appPort;
			mcpsReq.Req.Unconfirmed.fBuffer = appData;
			mcpsReq.Req.Unconfirmed.fBufferSize = appDataSize;
			mcpsReq.Req.Unconfirmed.Datarate = currentDrForNoAdr;
		}
		else
		{
			static uint32_t old = 0,new = 0;
			old = new;
			new = get_ms();
			
			BASE_PRINTF("confirmed uplink sending %d...\r\n",new -old);
			mcpsReq.Type = MCPS_CONFIRMED;
			mcpsReq.Req.Confirmed.fPort = appPort;
			mcpsReq.Req.Confirmed.fBuffer = appData;
			mcpsReq.Req.Confirmed.fBufferSize = appDataSize;
			mcpsReq.Req.Confirmed.NbTrials = confirmedNbTrials;
			mcpsReq.Req.Confirmed.Datarate = currentDrForNoAdr;
		}
	}
#ifdef __asr6601__
		DelayMs(5);
#endif
	if( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
	{
		return false;
	}
	return true;
}

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent( void )
{
	MibRequestConfirm_t mibReq;
	LoRaMacStatus_t status;

	TimerStop( &TxNextPacketTimer );

	mibReq.Type = MIB_NETWORK_JOINED;
	status = LoRaMacMibGetRequestConfirm( &mibReq );

	if( status == LORAMAC_STATUS_OK )
	{
		if( mibReq.Param.IsNetworkJoined == true )
		{
			deviceState = DEVICE_STATE_SEND;
			nextTx = true;
		}
		else
		{
			// Network not joined yet. Try to join again
			MlmeReq_t mlmeReq;
			mlmeReq.Type = MLME_JOIN;
			mlmeReq.Req.Join.DevEui = devEui;
			mlmeReq.Req.Join.AppEui = appEui;
			mlmeReq.Req.Join.AppKey = appKey;
			mlmeReq.Req.Join.NbTrials = 1;

			if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
			{
				deviceState = DEVICE_STATE_SLEEP;
			}
			else
			{
				deviceState = DEVICE_STATE_CYCLE;
			}
		}
	}
}

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
	if( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
	{
		switch( mcpsConfirm->McpsRequest )
		{
			case MCPS_UNCONFIRMED:
			{
				// Check Datarate
				// Check TxPower
				break;
			}
			case MCPS_CONFIRMED:
			{
				// Check Datarate
				// Check TxPower
				// Check AckReceived
				// Check NbTrials
				break;
			}
			case MCPS_PROPRIETARY:
			{
				break;
			}
			default:
				break;
		}
	}
	nextTx = true;
}

#if(LoraWan_RGB==1)
void turnOnRGB(uint32_t color,uint32_t time)
{
	uint8_t red,green,blue;
	red=(uint8_t)(color>>16);
	green=(uint8_t)(color>>8);
	blue=(uint8_t)color;
	// pinMode(Vext,OUTPUT);
	digitalWrite(Vext,LOW); //SET POWER
	DelayMs(1);
	pixels.begin(); // INITIALIZE RGB strip object (REQUIRED)
	pixels.clear(); // Set all pixel colors to 'off'
	pixels.setPixelColor(0, pixels.Color(red, green, blue));
	pixels.show();   // Send the updated pixel colors to the hardware.
	if(time>0)
	{
		DelayMs(time);
	}
}

void turnOffRGB(void)
{
	turnOnRGB(0,0);
#if defined(CubeCell_BoardPlus)||defined(CubeCell_GPS)
	if(isDispayOn == 0)
	{
		digitalWrite(Vext,HIGH);
	}
#else
	digitalWrite(Vext,HIGH);
#endif
}
#endif



void __attribute__((weak)) downLinkAckHandle()
{
	//printf("ack received\r\n");
}

void __attribute__((weak)) downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
	printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
	printf("+REV DATA:");
	for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
	{
		printf("%02X",mcpsIndication->Buffer[i]);
	}
	printf("\r\n");
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
int revrssi;
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
	if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
	{
		return;
	}
#if defined(CubeCell_BoardPlus)||defined(CubeCell_GPS)
	ifDisplayAck=1;
	revrssi=mcpsIndication->Rssi;
#endif
#if (LoraWan_RGB==1)
	turnOnRGB(COLOR_RECEIVED, 200);
	turnOffRGB();
#endif

	switch( mcpsIndication->McpsIndication )
	{
		case MCPS_UNCONFIRMED:
		{
			BASE_PRINTF( "received unconfirmed \r\n");
			break;
		}
		case MCPS_CONFIRMED:
		{
			BASE_PRINTF( "received confirmed \r\n");
			OnTxNextPacketTimerEvent( );
			break;
		}
		case MCPS_PROPRIETARY:
		{
			BASE_PRINTF( "received proprietary \r\n");
			break;
		}
		case MCPS_MULTICAST:
		{
			BASE_PRINTF( "received multicast \r\n");
			break;
		}
		default:
			break;
	}
	BASE_PRINTF( "downlink: rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi, (int)mcpsIndication->Snr,(int)mcpsIndication->RxDoneDatarate);

	if(mcpsIndication->AckReceived)
	{
		downLinkAckHandle();
	}

	if( mcpsIndication->RxData == true )
	{
		downLinkDataHandle(mcpsIndication);
	}

	// Check Multicast
	// Check Port
	// Check Datarate
	// Check FramePending
	if( mcpsIndication->FramePending == true )
	{
		// The server signals that it has pending data to be sent.
		// We schedule an uplink as soon as possible to flush the server.
		OnTxNextPacketTimerEvent( );
	}
	// Check Buffer
	// Check BufferSize
	// Check Rssi
	// Check Snr
	// Check RxSlot

	DelayMs(10);
}


void __attribute__((weak)) dev_time_updated()
{
	printf("device time updated\r\n");
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] mlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
	switch( mlmeConfirm->MlmeRequest )
	{
		case MLME_JOIN:
		{
			if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
			{

#if (LoraWan_RGB==1)
				turnOnRGB(COLOR_JOINED,500);
				turnOffRGB();
#endif
#if defined(CubeCell_BoardPlus)||defined(CubeCell_GPS)
				if(isDispayOn)
				{
					// LoRaWAN.displayJoined();
				}
#endif
				BASE_PRINTF("joined\r\n");
				
				//in PassthroughMode,do nothing while joined
				if(passthroughMode == false)
				{
					// Status is OK, node has joined the network
					deviceState = DEVICE_STATE_SEND;
				}
			}
			else
			{
				uint32_t rejoin_delay = 30000;
				BASE_PRINTF("join failed, join again at 30s later\r\n");
				DelayMs(5);
				TimerSetValue( &TxNextPacketTimer, rejoin_delay );
				TimerStart( &TxNextPacketTimer );
			}
			break;
		}
		case MLME_LINK_CHECK:
		{
			if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
			{
				// Check DemodMargin
				// Check NbGateways
			}
			break;
		}
		case MLME_DEVICE_TIME:
		{
			if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
			{
				dev_time_updated();
			}
			break;
		}
		default:
			break;
	}
	nextTx = true;
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
	switch( mlmeIndication->MlmeIndication )
	{
		case MLME_SCHEDULE_UPLINK:
		{// The MAC signals that we shall provide an uplink as soon as possible
			OnTxNextPacketTimerEvent( );
			break;
		}
		default:
			break;
	}
}


void lwan_dev_params_update( void )
{
#if defined( REGION_EU868 )
	LoRaMacChannelAdd( 3, ( ChannelParams_t )EU868_LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )EU868_LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )EU868_LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )EU868_LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )EU868_LC8 );
#elif defined( REGION_EU433 )
	LoRaMacChannelAdd( 3, ( ChannelParams_t )EU433_LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )EU433_LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )EU433_LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )EU433_LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )EU433_LC8 );
#elif defined( REGION_KR920 )
	LoRaMacChannelAdd( 3, ( ChannelParams_t )KR920_LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )KR920_LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )KR920_LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )KR920_LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )KR920_LC8 );
#elif defined( REGION_AS923 ) || defined( REGION_AS923_AS1 ) || defined( REGION_AS923_AS2 )
	LoRaMacChannelAdd( 2, ( ChannelParams_t )AS923_LC3 );
	LoRaMacChannelAdd( 3, ( ChannelParams_t )AS923_LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )AS923_LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )AS923_LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )AS923_LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )AS923_LC8 );
#endif

	MibRequestConfirm_t mibReq;

	mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
	mibReq.Param.ChannelsMask = userChannelsMask;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_CHANNELS_MASK;
	mibReq.Param.ChannelsMask = userChannelsMask;
	LoRaMacMibSetRequestConfirm(&mibReq);
}



LoRaMacPrimitives_t LoRaMacPrimitive;
LoRaMacCallback_t LoRaMacCallback;

void generateDeveuiByChipID()
{
	uint32_t uniqueId[2];
#if defined(__asr6601__)
	system_get_chip_id(uniqueId);
#elif defined(__asr650x__)
	CyGetUniqueId(uniqueId);
#elif defined(ESP_PLATFORM)
	uint64_t id = getID();
	uniqueId[0]=(uint32_t)(id>>32);
	uniqueId[1]=(uint32_t)id;
#endif
	for(int i=0;i<8;i++)
	{
		if(i<4)
			devEui[i] = (uniqueId[1]>>(8*(3-i)))&0xFF;
		else
			devEui[i] = (uniqueId[0]>>(8*(7-i)))&0xFF;
	}
}


void init(DeviceClass_t lorawanClass,LoRaMacRegion_t region)
{
	// LORAWANLOG;
	// BASE_PRINTF("LoRaWAN ");
	switch(region)
	{
		case LORAMAC_REGION_AS923_AS1:
			BASE_PRINTF("AS923(AS1:922.0-923.4MHz)\r\n");
			break;
		case LORAMAC_REGION_AS923_AS2:
			BASE_PRINTF("AS923(AS2:923.2-924.6MHz)\r\n");
			break;
		case LORAMAC_REGION_AS923:
			BASE_PRINTF("LORAMAC_REGION_AS923\r\n");
			break;
		case LORAMAC_REGION_AU915:
			BASE_PRINTF("AU915\r\n");
			break;
		case LORAMAC_REGION_CN470:
			BASE_PRINTF("CN470\r\n");
			break;
		case LORAMAC_REGION_CN779:
			BASE_PRINTF("CN779\r\n");
			break;
		case LORAMAC_REGION_EU433:
			BASE_PRINTF("EU433\r\n");
			break;
		case LORAMAC_REGION_EU868:
			BASE_PRINTF("EU868\r\n");
			break;
		case LORAMAC_REGION_KR920:
			BASE_PRINTF("KR920\r\n");
			break;
		case LORAMAC_REGION_IN865:
			BASE_PRINTF("IN865\r\n");
			break;
		case LORAMAC_REGION_US915:
			BASE_PRINTF("US915\r\n");
			break;
		case LORAMAC_REGION_US915_HYBRID:
			BASE_PRINTF("US915_HYBRID\r\n");
			break;
		default:
			printf("This frequency band option is not implemented!\r\n");
			exit(1);
			break;
	}
	BASE_PRINTF("Class %X start!\r\n\r\n",loraWanClass+10);

	if(region == LORAMAC_REGION_AS923_AS1 || region == LORAMAC_REGION_AS923_AS2)
		region = LORAMAC_REGION_AS923;
	MibRequestConfirm_t mibReq;

	LoRaMacPrimitive.MacMcpsConfirm = McpsConfirm;
	LoRaMacPrimitive.MacMcpsIndication = McpsIndication;
	LoRaMacPrimitive.MacMlmeConfirm = MlmeConfirm;
	LoRaMacPrimitive.MacMlmeIndication = MlmeIndication;
	LoRaMacCallback.GetBatteryLevel = BoardGetBatteryLevel;
	LoRaMacCallback.GetTemperatureLevel = NULL;

	LoRaMacInitialization( &LoRaMacPrimitive, &LoRaMacCallback,region);
	TimerStop( &TxNextPacketTimer );

	TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );

	mibReq.Type = MIB_ADR;
	mibReq.Param.AdrEnable = loraWanAdr;
	LoRaMacMibSetRequestConfirm( &mibReq );

	mibReq.Type = MIB_PUBLIC_NETWORK;
	mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
	LoRaMacMibSetRequestConfirm( &mibReq );

	lwan_dev_params_update();

	mibReq.Type = MIB_DEVICE_CLASS;
	LoRaMacMibGetRequestConfirm( &mibReq );
	
	if(loraWanClass != mibReq.Param.Class)
	{
		mibReq.Param.Class = loraWanClass;
		LoRaMacMibSetRequestConfirm( &mibReq );
	}

	deviceState = DEVICE_STATE_JOIN;

}


void join()
{
	if( overTheAirActivation )
	{
		BASE_PRINTF("joining...\r\n");
		MlmeReq_t mlmeReq;
		
		mlmeReq.Type = MLME_JOIN;

		mlmeReq.Req.Join.DevEui = devEui;
		mlmeReq.Req.Join.AppEui = appEui;
		mlmeReq.Req.Join.AppKey = appKey;
		mlmeReq.Req.Join.NbTrials = 1;

		if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
		{
			deviceState = DEVICE_STATE_SLEEP;
		}
		else
		{
			deviceState = DEVICE_STATE_CYCLE;
		}
	}
	else
	{
		MibRequestConfirm_t mibReq;

		mibReq.Type = MIB_NET_ID;
		mibReq.Param.NetID = LORAWAN_NETWORK_ID;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_DEV_ADDR;
		mibReq.Param.DevAddr = devAddr;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_NWK_SKEY;
		mibReq.Param.NwkSKey = nwkSKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_APP_SKEY;
		mibReq.Param.AppSKey = appSKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_NETWORK_JOINED;
		mibReq.Param.IsNetworkJoined = true;
		LoRaMacMibSetRequestConfirm( &mibReq );
		
		deviceState = DEVICE_STATE_SEND;
	}
}

void send()
{
	if( nextTx == true )
	{
		MibRequestConfirm_t mibReq;
		mibReq.Type = MIB_DEVICE_CLASS;
		LoRaMacMibGetRequestConfirm( &mibReq );

		if(loraWanClass != mibReq.Param.Class)
		{
			mibReq.Param.Class = loraWanClass;
			LoRaMacMibSetRequestConfirm( &mibReq );
		}
		nextTx = SendFrame( );
	}
}

void cycle(uint32_t dutyCycle)
{
	TimerSetValue( &TxNextPacketTimer, dutyCycle );
	TimerStart( &TxNextPacketTimer );
}

void loraSleep(DeviceClass_t classMode,uint8_t debugLevel)
{
	// Radio.IrqProcess( );
	// Mcusleep(classMode,debugLevel);
}
void setDataRateForNoADR(int8_t dataRate)
{
	defaultDrForNoAdr = dataRate;
}


// #ifndef ESP_PLATFORM
#ifdef ESP_PLATFORM

void ifskipjoin()
{
//if saved net info is OK in lorawan mode, skip join.
	if(checkNetInfo()&&modeLoraWan){
		;
		if(passthroughMode==false)
		{
			BASE_PRINTF("Wait 3s for user key to rejoin network");
			uint16_t i=0;
			// pinMode(USER_KEY,INPUT);
			while(i<=3000)
			{
				if(digitalRead(USER_KEY)==LOW)//if user key down, rejoin network;
				{
					netInfoDisable();
					pinMode(USER_KEY,OUTPUT);
					digitalWrite(USER_KEY,HIGH);
					return;
				}
				DelayMs(1);
				i++;
			}
			// pinMode(USER_KEY,OUTPUT);
			digitalWrite(USER_KEY,HIGH);
		}
#if(AT_SUPPORT)
		getDevParam();
#endif

		init(loraWanClass,loraWanRegion);
		getNetInfo();
		if(passthroughMode==false){
			BASE_PRINTF("User key not detected,Use reserved Net");
		}
		else{
			BASE_PRINTF("Use reserved Net");
		}
		if(passthroughMode==false)
		{
			int32_t temp=randr(0,appTxDutyCycle);
			;
			BASE_PRINTF("Next packet send %d ms later(random time from 0 to APP_TX_DUTYCYCLE)\r\n",temp);
			;
			cycle(temp);//send packet in a random time to avoid network congestion.
		}
		deviceState = DEVICE_STATE_SLEEP;
	}
}
#endif

#if defined(CubeCell_BoardPlus)||defined(CubeCell_GPS)
void displayJoining()
{
	display.setFont(ArialMT_Plain_16);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.clear();
	display.drawString(58, 22, "JOINING...");
	display.display();
}
void displayJoined()
{
	display.clear();
	display.drawString(64, 22, "JOINED");
	display.display();
	DelayMs(1000);
}
void displaySending()
{
    isDispayOn = 1;
	digitalWrite(Vext,LOW);
	display.init();
	display.setFont(ArialMT_Plain_16);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.clear();
	display.drawString(58, 22, "SENDING...");
	display.display();
	DelayMs(1000);
}
void displayAck()
{
    if(ifDisplayAck==0)
    {
    	return;
    }
    ifDisplayAck--;
	display.clear();
	display.drawString(64, 22, "ACK RECEIVED");
	char temp[10];
	sprintf(temp,"rssi: %d ",revrssi);
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_RIGHT);
	display.drawString(128, 0, temp);
	if(loraWanClass==CLASS_A)
	{
		display.setFont(ArialMT_Plain_10);
		display.setTextAlignment(TEXT_ALIGN_LEFT);
		display.drawString(28, 50, "Into deep sleep in 2S");
	}
	display.display();
	if(loraWanClass==CLASS_A)
	{
		DelayMs(2000);
		isDispayOn = 0;
		digitalWrite(Vext,HIGH);
		display.stop();
	}
}
void displayMcuInit()
{
	isDispayOn = 1;
	digitalWrite(Vext,LOW);
	display.init();
	display.setFont(ArialMT_Plain_16);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.clear();
	display.drawString(64, 11, "LORAWAN");
	display.drawString(64, 33, "STARTING");
	display.display();
	DelayMs(2000);
}
#endif


