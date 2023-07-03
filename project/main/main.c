#include "ESP32_sx1262_LoRaWAN.h"
#include "Region.h"
#include "sx126x-board.h"
#include "LoRaMac.h"
#include "data_conversion.h"
#include "mkfifo.h"
#include "iniparser.h"
#include "hdc1080.h"
#include "bmp180.h"
/* OTAA para*/
uint8_t devEui[] = { 0x22, 0x32, 0x33, 0x00, 0xaa, 0xaa, 0xaa, 0xaa };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 };
/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_C;

uint8_t debugLevel = 0;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = 0;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;

uint8_t confirmedNbTrials = 1;

enum eDeviceState_LoraWan deviceState;

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_EU868;

typedef enum 
{
    fixed_data =0,
    mkfifo_data,
    hdc1080_data,
    bmp180_data
}data_selection_num;
data_selection_num data_selection = fixed_data;
uint8_t fixed_data_arr[128] ={0};
uint8_t fixed_data_len = 0;
void loop(void);
void read_config(void);
void read_hannel(const uint16_t start_ch_num,const uint16_t end_ch_num);
static void prepareTxFrame( uint8_t port ,data_selection_num data_selection);
static void sensor_init(data_selection_num data_selection);

char io_buff[2048];
int main(int argc, char const *argv[])
{
  memset(io_buff,'\0',sizeof(io_buff));
  setvbuf(stdout,io_buff,_IOLBF,sizeof(io_buff));
  read_config();
  sensor_init(data_selection);
  printf("begin\n");
  deviceState = DEVICE_STATE_INIT;
  loop();
  printf("end \n");
  return 0;  
}


void loop(void)
{
  while(1)
  {
    switch( deviceState )
    {
      case DEVICE_STATE_INIT:
      {
        init(loraWanClass,loraWanRegion);
        break;
      }
      case DEVICE_STATE_JOIN:
      {
        join();
        break;
      }
      case DEVICE_STATE_SEND:
      {
        prepareTxFrame(appPort,data_selection);
        send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
      case DEVICE_STATE_CYCLE:
      {     
        txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
        cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
      case DEVICE_STATE_SLEEP:
      {
        loraSleep(loraWanClass,debugLevel);
        break;
      }
      default:
      {
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    }
  }
}

static void prepareTxFrame( uint8_t port ,data_selection_num data_selection)
{
    char buf[128] ={0};
    uint8_t len =0;
    double hdc1080_temperature =0;
    double hdc1080_humidity =0;
    float bmp180_temperature,bmp180_altitide;
    int32_t bmp180_pressure;         
    appPort = port;
    switch (data_selection)
    {
        case fixed_data:
            for(int i = 0;i <fixed_data_len; i++)
            {
                appData[i] =fixed_data_arr[i]; 
            }
            appDataSize = fixed_data_len;
            break;
        case mkfifo_data:
            read_mkfifo(buf,&len);
            if(len ==0)
            {
                appPort = 2; //error data
                appDataSize = 4;
                appData[0] = 0x00;
                appData[1] = 0x01;
                appData[2] = 0x02;
                appData[3] = 0x03;
            }
            else
            {
                appDataSize =len/2;
                str_to_array(buf,appData,strlen(buf));    
            }
 
            break;
        case hdc1080_data:
            hdc1080_read_temperature(&hdc1080_temperature);
            hdc1080_read_humidity(&hdc1080_humidity);
            appDataSize=4;
            appData[0] = (uint8_t)((((uint16_t)(hdc1080_temperature*100))&0xFF00)>>8);
            appData[1] = (uint8_t)(((uint16_t)(hdc1080_temperature*100))&0x00FF);
            appData[2] = (uint8_t)((((uint16_t)(hdc1080_humidity*100))&0xFF00)>>8);
            appData[3] = (uint8_t)(((uint16_t)(hdc1080_humidity*100))&0x00FF);
            break;
        case bmp180_data:
            bmp180_convert(&bmp180_temperature, &bmp180_pressure);
            bmp180_altitude( &bmp180_altitide);
            appDataSize=8;
            appData[0] = (uint8_t)((((uint16_t)(bmp180_temperature*100))&0xFF00)>>8);
            appData[1] = (uint8_t)(((uint16_t)(bmp180_temperature*100))&0x00FF);
            appData[2] = (uint8_t)((((uint16_t)(bmp180_altitide*100))&0xFF00)>>8);
            appData[3] = (uint8_t)(((uint16_t)(bmp180_altitide*100))&0x00FF);
            appData[4] = (uint8_t)(((uint32_t)bmp180_pressure)&0x000000FF);
            appData[5] = (uint8_t)((((uint32_t)bmp180_pressure)&0x0000FF00)>>8);
            appData[6] = (uint8_t)((((uint32_t)bmp180_pressure)&0x00FF0000)>>16);
            appData[7] = (uint8_t)((((uint32_t)bmp180_pressure)&0xFF000000)>>24);
            break;
        default:
            appPort = 2; //error data
            appDataSize = 4;
            appData[0] = 0x00;
            appData[1] = 0x01;
            appData[2] = 0x02;
            appData[3] = 0x03;
            break;
    }

}

static void sensor_init(data_selection_num data_selection)
{
    switch (data_selection)
    {
        case fixed_data:
            /* code */
            break;
        case mkfifo_data:
            mkfifo_init();
            break;
        case hdc1080_data:
            hdc1080_init(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
            break;
        case bmp180_data:
            bmp180_init();
            break;  
        default:
            break;
    }
}

void read_config(void)
{
    dictionary *ini;
    int num = 0;
    ini = iniparser_load("../project/config/config.ini");//parser the file
    if(ini == NULL)
    {
        fprintf(stderr,"can not open %s","config.ini");
        exit(EXIT_FAILURE);
    }
    num = iniparser_getint(ini,"LORAWAN_REGION:region",-1);
    loraWanRegion = num;


    //lora modepreamble_Len
    num = iniparser_getint(ini,"LORAWAN_METHOD:mode",-1);
    if(num ==0)
    {
        //OTAA mode
        overTheAirActivation = true;
        const char *str_dev_eui = iniparser_getstring(ini,"LORAWAN_METHOD:dev_eui","null");
        if(strlen(str_dev_eui)==16)
        {
           str_to_array(str_dev_eui,devEui,strlen(str_dev_eui));
        }
        else
        {
            perror("dev_eui input error:");
            exit(1);
        }

        const char *str_app_eui = iniparser_getstring(ini,"LORAWAN_METHOD:app_eui","null");
        if(strlen(str_app_eui)==16)  
        {
            str_to_array(str_app_eui,appEui,strlen(str_app_eui));
        }
        else
        {
            perror("app_eui input error:");
            exit(1);
        }
        const char *str_app_key = iniparser_getstring(ini,"LORAWAN_METHOD:app_key","null"); 
        if(strlen(str_app_key)==32)  
        {
            str_to_array(str_app_key,appKey,strlen(str_app_key));
        }
        else
        {
            perror("app_key input error:");
            exit(1);
        }
        
    }
    else
    {
        //ABP mode
        overTheAirActivation = false;
        const char *str_nwk_skey = iniparser_getstring(ini,"LORAWAN_METHOD:nwk_skey","null");
        if(strlen(str_nwk_skey)==32)  
        {
            str_to_array(str_nwk_skey,nwkSKey,strlen(str_nwk_skey));
        }
        else
        {
            perror("nwk_skey input error:");
            exit(1);
        }
        const char *str_app_skey = iniparser_getstring(ini,"LORAWAN_METHOD:app_skey","null");
        if(strlen(str_app_skey)==32)  
        {
            str_to_array(str_app_skey,appSKey,strlen(str_app_skey));
        }
        else
        {
            perror("app_skey input error:");
            exit(1);
        }

        const char *str_dev_addr = iniparser_getstring(ini,"LORAWAN_METHOD:dev_addr","null");
        if(strlen(str_dev_addr)==8)  
        {
            str_to_int(str_dev_addr,&devAddr,strlen(str_dev_addr)); 
        }
        else
        {
            perror("dev_addr input error:");
            exit(1);
        }
    }


    num = iniparser_getint(ini,"LORAWAN_CLASS:class",-1);
    loraWanClass = num;

    num = iniparser_getint(ini,"LORAWAN_PREAMBLE_LENGTH:preamble_Len",-1);
    preamble_Len = num;

    num = iniparser_getint(ini,"LORAWAN_DEBUG_LEVEL:debug",-1);
    debugLevel = num;

    uint16_t start_ch_num = iniparser_getint(ini,"LORAWAN_CHANNEL_MASK:channel_start",-1);
    uint16_t end_ch_num = iniparser_getint(ini,"LORAWAN_CHANNEL_MASK:channel_end",-1);
    read_hannel(start_ch_num,end_ch_num);

    uint32_t duty_cycle = iniparser_getint(ini,"LORAWAN_DUTY_CYCLE:duty_cycle",-1);
    appTxDutyCycle = duty_cycle;

    const char *str_tx_frame = iniparser_getstring(ini,"LORAWAN_TX_FRAME:tx_frame","null");

    data_selection = iniparser_getint(ini,"LORAWAN_DATA_SELECTION:data_selection",-1);
    if(((strlen(str_tx_frame))%2) ==0)
    {
        fixed_data_len =  strlen(str_tx_frame)/2;
        str_to_array(str_tx_frame,fixed_data_arr,strlen(str_tx_frame));
    }
    else
    {
        perror("tx_frame input error:");
        exit(1);
    }

    num = iniparser_getint(ini,"LORAWAN_APP_PORT:app_port",-1);
    appPort = num;

    const char *str_adr = iniparser_getstring(ini,"LORAWAN_ADR:adr","null");
    if(strcmp("enable",str_adr) ==0)
    {
        loraWanAdr = true;
    }
    else
    {
        loraWanAdr = false;
    }

    const char *str_tx_confirmed = iniparser_getstring(ini,"LORAWAN_TX_CONFIRMED:tx_confirmed","null");
    if(strcmp("enable",str_tx_confirmed) ==0)
    {
        isTxConfirmed = true;
    }
    else
    {
         isTxConfirmed = false;
    }

    const char *str_license = iniparser_getstring(ini,"LORAWAN_LICENSE:license","null");
    for (int i = 0, j = 0, k = 0; i < strlen(str_license);)
    {
        if ((str_license[i] == '0') && (str_license[i + 1] == 'x'))
        {
            i += 2;
            k = 28;
            while ((str_license[i] != ',') && (i < strlen(str_license)))
            {
                if ((str_license[i] >= '0') && (str_license[i] <= '9'))
                {
                    config_licen[j] += (((uint32_t)(str_license[i] - '0')) << k);
                }
                else if ((str_license[i] >= 'A') && (str_license[i] <= 'F'))
                {
                    config_licen[j] += (((uint32_t)(str_license[i] - 55)) << k);
                }
                else if((str_license[i] >= 'a') && (str_license[i] <= 'f'))
                {
                    config_licen[j] += (((uint32_t)(str_license[i] - 87)) << k);
                }
                else
                {
                    printf("License input error\n");
                }
                i++;
                k -= 4;
            }
            config_licen[j] = (config_licen[j] >> (k+4));  //There may be cases where the high bit is 0. 0xC2B0BE0
            j++;
        }
        else
        {
            i++;
        }
    }
    iniparser_freedict(ini);//free dirctionary obj

}



void read_hannel(const uint16_t start_ch_num,const uint16_t end_ch_num)
{
    const uint8_t interval = 16;
    uint16_t temp = interval;
    uint8_t count = 0;
    uint8_t start_interval = 0;
    uint8_t end_interval = 0;
    if (start_ch_num > end_ch_num)
    {
        printf("LoRaWAN channels mask fault\r\n");
        return;
    }

    memset(userChannelsMask, 0, 6*sizeof(uint16_t));

    while (temp <= start_ch_num)
    {
        start_interval += 1;
        temp = interval * (start_interval + 1);
    }

    temp = interval;
    while (temp <= end_ch_num)
    {
        end_interval += 1;
        temp = interval * (end_interval + 1);
    }

    if (start_interval == end_interval)
    {
        temp = 0x00;
        for (int i = (start_ch_num % interval); i <= (end_ch_num % interval); i++)
        {
            temp += (1 << i);
        }
        userChannelsMask[start_interval] = temp;
    }
    else
    {
        temp = 0x00;
        for (int i = (start_ch_num % interval); i <= 15; i++)
        {
            temp += (1 << i);
        }
        userChannelsMask[start_interval] = temp;


        temp = 0x00;
        for (int i = 0; i <= (end_ch_num % interval); i++)
        {
            temp += (1 << i);
        }
        userChannelsMask[end_interval] = temp;
        count = start_interval + 1;
        while (count < end_interval)
        {
            userChannelsMask[count] = 0xFFFF;
            count++;
        }
    }
}
