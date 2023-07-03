#!/bin/env bash

confile=config.ini
# class=1
# mode=1
# dev_eui=2232330000888803
# class=2
sed -i 's/[[:space:]]//g' $confile  #去除所有空格
# sed -i 's/mode=[0-9];/mode=1;/g' $confile
# sed -i "s/class=[0-9];/class=$class;/g" $confile
# sed -i "s/dev_eui=[0-9]*;/dev_eui=$dev_eui;/g" $confile

# region=0
# eval $(awk -F'=' '/region/{printf("region=%d\n") ,$2}' $confile)

# echo $region 
# REGION_AS923=ON

LORAWAN_REGION=$(whiptail  --title  "HelTec LoRaWAN Config"  --radiolist \
"LoRaWAN region choices?"  15  60  4  \
"0"  "LORAMAC_REGION_AS923"  OFF \
"1"  "LORAMAC_REGION_AU915"  OFF \
"2"  "LORAMAC_REGION_CN470"  OFF \
"3"  "LORAMAC_REGION_CN779"  OFF \
"4"  "LORAMAC_REGION_EU433"  OFF \
"5"  "LORAMAC_REGION_EU868"  ON \
"6"  "LORAMAC_REGION_KR920"  OFF \
"7"  "LORAMAC_REGION_IN865"  OFF \
"8"  "LORAMAC_REGION_US915"  OFF \
"9"  "LORAMAC_REGION_US915_HYBRID"  OFF \
"10"  "LORAMAC_REGION_AU915_SB2"  OFF \
"11"  "LORAMAC_REGION_AS923_AS1"  OFF \
"12"  "LORAMAC_REGION_AS923_AS2"  OFF  3>&1  1>&2  2>&3 )
 
exitstatus=$?
if  [ $exitstatus  =  0  ]; then
     echo  "The chosen region is:"  $LORAWAN_REGION
     sed -i "s/region=[a-zA-Z0-9]*;/region=$LORAWAN_REGION;/g" $confile
else
     echo  "You chose Cancel." $exitstatus
fi

##################
LORAWAN_CLASS=$(whiptail  --title  "HelTec LoRaWAN Config"  --radiolist \
"LoRaWAN class choices?"  15  60  4  \
"0"  "CLASS_A"  ON  \
"1"  "CLASS_B"  OFF \
"2"  "CLASS_C"  OFF  3>&1  1>&2  2>&3 )
 
exitstatus=$?
if  [ $exitstatus  =  0  ]; then
     echo  "The chosen class is:"  $LORAWAN_CLASS
     sed -i "s/class=[a-zA-Z0-9]*;/class=$LORAWAN_CLASS;/g" $confile
else
     echo  "You chose Cancel." $exitstatus
fi


OTAA_ABP=$(whiptail  --title  "HelTec LoRaWAN Config"  --radiolist \
"LoRaWAN network access method?"  15  60  4  \
"0"  "OTAA"  ON  \
"1"  "ABP"  OFF   3>&1  1>&2  2>&3 )
 
exitstatus=$?
if  [ $exitstatus  =  0  ]; then
     echo  "The chosen mode is:"  $OTAA_ABP
     sed -i "s/mode=[a-zA-Z0-9]*;/mode=$OTAA_ABP;/g" $confile
else
     echo  "You chose Cancel." $exitstatus
fi
if [ $OTAA_ABP = 0 ];then
     DEV_EUI=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "DEV_EUI:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     APP_EUI=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_EUI:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     APP_KEY=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_KEY:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     echo DEV_EUI $DEV_EUI
     echo APP_EUI $APP_EUI
     echo APP_KEY $APP_KEY
     if [  -n "$DEV_EUI" ]; then  
        sed -i "s/dev_eui=[a-zA-Z0-9]*;/dev_eui=$DEV_EUI;/g" $confile
     fi
     
     if [  -n "$APP_EUI" ]; then  
        sed -i "s/app_eui=[a-zA-Z0-9]*;/app_eui=$APP_EUI;/g" $confile
     fi
     
     if [  -n "$APP_KEY" ]; then  
        sed -i "s/appkey=[a-zA-Z0-9]*;/appkey=$APP_KEY;/g" $confile
     fi
     
else
     NWK_SKEY=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "NWK_SKEY:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     APP_SKEY=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_SKEY:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     DEV_ADDR=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "DEV_ADDR:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
     echo NWK_SKEY $NWK_SKEY
     echo APP_SKEY $APP_SKEY
     echo DEV_ADDR $DEV_ADDR
     if [  -n "$NWK_SKEY" ]; then  
        sed -i "s/nwk_skey=[a-zA-Z0-9]*;/nwk_skey=$NWK_SKEY;/g" $confile
     fi

     if [  -n "$APP_SKEY" ]; then  
        sed -i "s/app_skey=[a-zA-Z0-9]*;/app_skey=$APP_SKEY;/g" $confile
     fi

     if [  -n "$DEV_ADDR" ]; then  
        sed -i "s/dev_addr=[a-zA-Z0-9]*;/dev_addr=$DEV_ADDR;/g" $confile
     fi
     
    
     
fi


DEBUG_LEVEL=$(whiptail  --title  "HelTec LoRaWAN Config"  --radiolist \
"LoRaWAN class choices?"  15  60  4  \
"0"  "PRINT_ERROR"  OFF  \
"1"  "PRINT_BASIC_INFO"  ON  \
"2"  "PRINT_FREQENCY"    OFF  \
"3"  "PRINT_FREQENCY_DIO" OFF   3>&1  1>&2  2>&3 )
 
exitstatus=$?
if  [ $exitstatus  =  0  ]; then
     echo  "The chosen DEBUG_LEVEL is:"  $DEBUG_LEVEL
     sed -i "s/debug=[a-zA-Z0-9]*;/debug=$DEBUG_LEVEL;/g" $confile
else
     echo  "You chose Cancel." $exitstatus
fi


START_CH_NUM=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "START_CH_NUM:Please enter a positive decimal integer." 10 60  3>&1 1>&2 2>&3)
END_CH_NUM=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "END_CH_NUM:Please enter a positive decimal integer." 10 60  3>&1 1>&2 2>&3)
echo START_CH_NUM $START_CH_NUM
echo END_CH_NUM $END_CH_NUM
if [  -n "$START_CH_NUM" ]; then  
    sed -i "s/channel_start=[a-zA-Z0-9]*;/channel_start=$START_CH_NUM;/g" $confile
fi
if [  -n "$END_CH_NUM" ]; then  
    sed -i "s/channel_end=[a-zA-Z0-9]*;/channel_end=$END_CH_NUM;/g" $confile
fi



APP_TX_DUTY_CYCLE=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_TX_DUTY_CYCLE:Please enter a positive decimal integer.unit:ms" 10 60  3>&1 1>&2 2>&3)
echo APP_TX_DUTY_CYCLE $APP_TX_DUTY_CYCLE
if [  -n "$APP_TX_DUTY_CYCLE" ]; then  
    sed -i "s/duty_cycle=[a-zA-Z0-9]*;/duty_cycle=$APP_TX_DUTY_CYCLE;/g" $confile
fi

APP_PORT=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_PORT:Please enter a positive decimal integer.unit:ms" 10 60  3>&1 1>&2 2>&3)
echo APP_PORT $APP_PORT
if [  -n "$APP_PORT" ]; then  
    sed -i "s/app_port=[a-zA-Z0-9]*;/app_port=$APP_PORT;/g" $confile
fi


APP_TX_FRAME=$(whiptail --title "HelTec LoRaWAN Config" --inputbox "APP_TX_FRAME:Please enter a hexadecimal number (0-F). If it is the number 1, please enter 01." 10 60  3>&1 1>&2 2>&3)
echo APP_TX_FRAME $APP_TX_FRAME
if [  -n "$APP_TX_FRAME" ]; then  
    sed -i "s/tx_frame=[a-zA-Z0-9]*;/tx_frame=$APP_TX_FRAME;/g" $confile
fi



if (whiptail --title "HelTec LoRaWAN Config" --yes-button "enable" --no-button "disable"  --yesno "Is the ADR function enabled?" 10 60) then
    echo "You chose enable status was $?."
    sed -i "s/adr=[a-zA-Z0-9]*;/adr=enable;/g" $confile
else
    echo "You chose disable. Exit status was $?."
    sed -i "s/adr=[a-zA-Z0-9]*;/adr=disenable;/g" $confile
fi

if (whiptail --title "HelTec LoRaWAN Config" --yes-button "enable" --no-button "disable"  --yesno "Is the TX_CONFIRMED function enabled?" 10 60) then
    echo "You chose enable status was $?."
    sed -i "s/tx_confirmed=[a-zA-Z0-9]*;/tx_confirmed=enable;/g" $confile
else
    echo "You chose disable. Exit status was $?."
    sed -i "s/tx_confirmed=[a-zA-Z0-9]*;/tx_confirmed=disenable;/g" $confile
fi
