#!/usr/bin/env bash 
K_CONFIGFILE=".config"
INI_CONFIGFILE='../project/config/config.ini'
sed -i 's/[[:space:]]//g' $INI_CONFIGFILE  #去除所有空格

#Configure LoRaWAN frequency band
eval $(awk -F'=' '/CONFIG_REGION_[a-zA-Z0-9_-]*=y/{printf("REGION=%s\n") ,$1}' $K_CONFIGFILE)
# echo $REGION 
eval $(awk -F'=' '/;'"${REGION: 7}"'=[a-zA-Z0-9_-]*/{printf("REGION=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $REGION
sed -i "s/region=[a-zA-Z0-9]*;/region=$REGION;/g" $INI_CONFIGFILE

#Configure LoRaWAN class 
eval $(awk -F'=' '/CONFIG_CLASS_[a-zA-Z0-9_-]*=y/{printf("CLASS=%s\n") ,$1}' $K_CONFIGFILE)
# echo $CLASS 
eval $(awk -F'=' '/;'"${CLASS: 7}"'=[a-zA-Z0-9_-]*/{printf("CLASS=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $CLASS
sed -i "s/class=[a-zA-Z0-9]*;/class=$CLASS;/g" $INI_CONFIGFILE

#Configure LORAWAN_PREAMBLE_LENGTH ;PREAMBLE_LENGTH_
eval $(awk -F'=' '/CONFIG_PREAMBLE_LENGTH_[a-zA-Z0-9_-]*=y/{printf("PREAMBLE=%s\n") ,$1}' $K_CONFIGFILE)
# echo $PREAMBLE 
eval $(awk -F'=' '/;'"${PREAMBLE: 7}"'=[a-zA-Z0-9_-]*/{printf("PREAMBLE=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $PREAMBLE
sed -i "s/preamble_Len=[a-zA-Z0-9]*;/preamble_Len=$PREAMBLE;/g" $INI_CONFIGFILE


#Configure LoRaWAN mode  OTAA_ABP
read OTAA_ABP < <(awk -F'=' '/^CONFIG_MODE_OTAA/{print $1}' $K_CONFIGFILE)
# echo $OTAA_ABP 

if [ $? -eq 0 ];then
    # echo OTAA
    eval $(awk -F'=' '/;'"${OTAA_ABP: 7}"'=[a-zA-Z0-9_-]*/{printf("OTAA_ABP=%d\n") ,$2}' $INI_CONFIGFILE)
    # echo $OTAA_ABP
    sed -i "s/mode=[a-zA-Z0-9_-]*;/mode=$OTAA_ABP;/g" $INI_CONFIGFILE
    
    eval $(awk -F'=' '/^CONFIG_DEV_EUI/{printf("DEV_EUI=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $DEV_EUI
    eval $(awk -F'=' '/^CONFIG_APP_EUI/{printf("APP_EUI=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $APP_EUI
    eval $(awk -F'=' '/^CONFIG_APP_KEY/{printf("APP_KEY=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $APP_KEY
    sed -i "s/dev_eui=[a-zA-Z0-9]*;/dev_eui=$DEV_EUI;/g" $INI_CONFIGFILE
    sed -i "s/app_eui=[a-zA-Z0-9]*;/app_eui=$APP_EUI;/g" $INI_CONFIGFILE
    sed -i "s/app_key=[a-zA-Z0-9]*;/app_key=$APP_KEY;/g" $INI_CONFIGFILE

else
    # echo ABP
    OTAA_ABP=CONFIG_MODE_ABP
    eval $(awk -F'=' '/;'"${OTAA_ABP: 7}"'=[a-zA-Z0-9_-]*/{printf("OTAA_ABP=%d\n") ,$2}' $INI_CONFIGFILE)
    # echo $OTAA_ABP
    sed -i "s/mode=[a-zA-Z0-9_-]*;/mode=$OTAA_ABP;/g" $INI_CONFIGFILE
    
    eval $(awk -F'=' '/^CONFIG_NWK_SKEY/{printf("NWK_SKEY=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $NWK_SKEY
    eval $(awk -F'=' '/^CONFIG_APP_SKEY/{printf("APP_SKEY=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $APP_SKEY
    eval $(awk -F'=' '/^CONFIG_DEV_ADDR/{printf("DEV_ADDR=%s\n") ,$2}' $K_CONFIGFILE)
    # echo $DEV_ADDR
    sed -i "s/nwk_skey=[a-zA-Z0-9_-]*;/nwk_skey=$NWK_SKEY;/g" $INI_CONFIGFILE
    sed -i "s/app_skey=[a-zA-Z0-9_-]*;/app_skey=$APP_SKEY;/g" $INI_CONFIGFILE
    sed -i "s/dev_addr=[a-zA-Z0-9_-]*;/dev_addr=$DEV_ADDR;/g" $INI_CONFIGFILE

fi



#Configure LoRaWAN debug 
eval $(awk -F'=' '/CONFIG_PRINT_[a-zA-Z0-9_-]*=y/{printf("DEBUG=%s\n") ,$1}' $K_CONFIGFILE)
# echo $DEBUG WWW
eval $(awk -F'=' '/;'"${DEBUG: 7}"'=[a-zA-Z0-9_-]*/{printf("DEBUG=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $DEBUG
sed -i "s/debug=[a-zA-Z0-9_-]*;/debug=$DEBUG;/g" $INI_CONFIGFILE


#Configure LoRaWAN channel 
eval $(awk -F'=' '/CONFIG_START_CH_NUM/{printf("START_CH_NUM=%s\n") ,$2}' $K_CONFIGFILE)
sed -i "s/channel_start=[a-zA-Z0-9_-]*;/channel_start=$START_CH_NUM;/g" $INI_CONFIGFILE
eval $(awk -F'=' '/CONFIG_END_CH_NUM/{printf("END_CH_NUM=%s\n") ,$2}' $K_CONFIGFILE)
sed -i "s/channel_end=[a-zA-Z0-9_-]*;/channel_end=$END_CH_NUM;/g" $INI_CONFIGFILE


#Configure LoRaWAN duty cycle 
eval $(awk -F'=' '/CONFIG_APP_TX_DUTY_CYCLE/{printf("APP_TX_DUTY_CYCLE=%s\n") ,$2}' $K_CONFIGFILE)
sed -i "s/duty_cycle=[a-zA-Z0-9_-]*;/duty_cycle=$APP_TX_DUTY_CYCLE;/g" $INI_CONFIGFILE


#Configure LoRaWAN DATA_SELECTION 
eval $(awk -F'=' '/[a-zA-Z0-9_-]*DATA=y/{printf("DATA_SELECTION=%s\n") ,$1}' $K_CONFIGFILE)
eval $(awk -F'=' '/;'"${DATA_SELECTION: 7}"'=[a-zA-Z0-9-_]*/{printf("DATA_SELECTION=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "s/data_selection=[a-zA-Z0-9_-]*;/data_selection=$DATA_SELECTION;/g" $INI_CONFIGFILE
if [ $DATA_SELECTION -eq 0 ];then
    #Configure LoRaWAN tx frame
    eval $(awk -F'=' '/CONFIG_APP_TX_FRAME/{printf("APP_TX_FRAME=%s\n") ,$2}' $K_CONFIGFILE)
    sed -i "s/tx_frame=[a-zA-Z0-9_-]*;/tx_frame=$APP_TX_FRAME;/g" $INI_CONFIGFILE
fi

#Configure LoRaWAN app port
eval $(awk -F'=' '/CONFIG_APP_PORT/{printf("APP_PORT=%s\n") ,$2}' $K_CONFIGFILE)
sed -i "s/app_port=[a-zA-Z0-9_-]*;/app_port=$APP_PORT;/g" $INI_CONFIGFILE


#Configure LoRaWAN adr
ADR_ENABLE=no
eval $(awk -F'=' '/CONFIG_ADR_ENABLE=/{printf("ADR_ENABLE=%s\n") ,$2}' $K_CONFIGFILE)
if [ $ADR_ENABLE = 'y' ];then
    sed -i "s/adr=[a-zA-Z0-9]*;/adr=enable;/g" $INI_CONFIGFILE
else
    sed -i "s/adr=[a-zA-Z0-9_-]*;/adr=disenable;/g" $INI_CONFIGFILE
fi

#Configure LoRaWAN tx confirmed
TX_CONFIRMED=no
eval $(awk -F'=' '/CONFIG_TX_CONFIRMED=/{printf("TX_CONFIRMED=%s\n") ,$2}' $K_CONFIGFILE)
if [ $TX_CONFIRMED = 'y' ];then
    sed -i "s/tx_confirmed=[a-zA-Z0-9_-]*;/tx_confirmed=enable;/g" $INI_CONFIGFILE
else
    sed -i "s/tx_confirmed=[a-zA-Z0-9_-]*;/tx_confirmed=disenable;/g" $INI_CONFIGFILE
fi

#Configure HelTec LoRaWAN license
read LICENSE < <(awk -F'=' '/^CONFIG_HELTEC_LICENSE/{print $2}' $K_CONFIGFILE)
LICENSE=${LICENSE:1}
sed -i "s/license=[a-zA-Z0-9\"-\",-]*;/license=${LICENSE%\"*};/g" $INI_CONFIGFILE