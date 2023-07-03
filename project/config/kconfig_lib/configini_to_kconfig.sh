#!/usr/bin/env bash 
K_CONFIGFILE=".config"
INI_CONFIGFILE='../project/config/config.ini'

sed -i 's/[[:space:]]//g' $INI_CONFIGFILE  #Remove all spaces
# sed -i '/[[:space:]]*^$/d' $K_CONFIGFILE  #Delete all blank lines

#Configure LoRaWAN frequency band
eval $(awk -F'=' '/region=[a-zA-Z0-9-_]*/{printf("REGION=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $REGION
read REGION < <(awk -F'=' '/;REGION_[a-zA-Z0-9-_]*='"${REGION}"'/{print $1}' $INI_CONFIGFILE)
# echo ${REGION: 1}
sed -i "s/CONFIG_REGION_[a-zA-Z0-9_-]*=y/CONFIG_${REGION: 1}=y/g" $K_CONFIGFILE


#Configure LoRaWAN class 
eval $(awk -F'=' '/class=[a-zA-Z0-9-_]*/{printf("CLASS=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $CLASS
read CLASS < <(awk -F'=' '/;CLASS_[a-zA-Z0-9-_]*='"${CLASS}"'/{print $1}' $INI_CONFIGFILE)
# echo $CLASS
# echo ${CLASS: 7}
sed -i "s/CONFIG_CLASS_[a-zA-Z0-9_-]*=y/CONFIG_CLASS_${CLASS: 7}=y/g" $K_CONFIGFILE


#Configure LORAWAN_PREAMBLE_LENGTH
eval $(awk -F'=' '/preamble_Len=[a-zA-Z0-9-_]*/{printf("PREAMBLE=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $PREAMBLE
read PREAMBLE < <(awk -F'=' '/;PREAMBLE_LENGTH_[a-zA-Z0-9-_]*='"${PREAMBLE}"'/{print $1}' $INI_CONFIGFILE)
# echo $PREAMBLE
sed -i "s/CONFIG_PREAMBLE_LENGTH_[a-zA-Z0-9_-]*=y/CONFIG_PREAMBLE_LENGTH_${PREAMBLE: 17}=y/g" $K_CONFIGFILE


#Configure LoRaWAN mode  OTAA_ABP
eval $(awk -F'=' '/mode=[a-zA-Z0-9-_]*/{printf("OTAA_ABP=%d\n") ,$2}' $INI_CONFIGFILE)
if [ $OTAA_ABP -eq 0 ];then
    # echo OTAA
    read CLASS < <(awk -F'=' '/;MODE_[a-zA-Z0-9-_]*='"${OTAA_ABP}"'/{print $1}' $INI_CONFIGFILE)
    # echo ${CLASS: 7}
    sed -i "s/CONFIG_MODE_[a-zA-Z0-9_-]*=y/CONFIG_MODE_${CLASS: 6}=y/g" $K_CONFIGFILE

    #Annotate ABP parameters
    sed -i "/CONFIG_NWK_SKEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    sed -i "/CONFIG_APP_SKEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    sed -i "/CONFIG_DEV_ADDR=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
   
    read CONFIG_DEV_EUI < <(awk -F'=' '/^CONFIG_DEV_EUI=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_DEV_EUI ];then
        sed -i "/CONFIG_DEV_EUI=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
        sed -i "/CONFIG_APP_EUI=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
        sed -i "/CONFIG_APP_KEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    fi
        read CONFIG_DEV_EUI < <(awk -F'=' '/dev_eui=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_DEV_EUI=\"${CONFIG_DEV_EUI%;*}\"" >> $K_CONFIGFILE

        read CONFIG_APP_EUI < <(awk -F'=' '/app_eui=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_APP_EUI=\"${CONFIG_APP_EUI%;*}\"" >> $K_CONFIGFILE

        read CONFIG_APP_KEY < <(awk -F'=' '/app_key=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_APP_KEY=\"${CONFIG_APP_KEY%;*}\"" >> $K_CONFIGFILE
    

else
    # echo ABP
    read CLASS < <(awk -F'=' '/;MODE_[a-zA-Z0-9-_]*='"${OTAA_ABP}"'/{print $1}' $INI_CONFIGFILE)
    # echo ${CLASS: 7}
    sed -i "s/CONFIG_MODE_[a-zA-Z0-9_-]*=y/CONFIG_MODE_${CLASS: 6}=y/g" $K_CONFIGFILE

    sed -i "/CONFIG_DEV_EUI=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    sed -i "/CONFIG_APP_EUI=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    sed -i "/CONFIG_APP_KEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE

    read CONFIG_NWK_SKEY < <(awk -F'=' '/^CONFIG_NWK_SKEY=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_NWK_SKEY ];then
        #Annotate ABP parameters
        sed -i "/CONFIG_NWK_SKEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
        sed -i "/CONFIG_APP_SKEY=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
        sed -i "/CONFIG_DEV_ADDR=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
    fi
        read CONFIG_NWK_SKEY < <(awk -F'=' '/nwk_skey=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_NWK_SKEY=\"${CONFIG_NWK_SKEY%;*}\"" >> $K_CONFIGFILE

        read CONFIG_APP_SKEY < <(awk -F'=' '/app_skey=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_APP_SKEY=\"${CONFIG_APP_SKEY%;*}\"" >> $K_CONFIGFILE

        read CONFIG_DEV_ADDR < <(awk -F'=' '/dev_addr=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
        echo "CONFIG_DEV_ADDR=\"${CONFIG_DEV_ADDR%;*}\"" >> $K_CONFIGFILE
fi


#Configure LoRaWAN upload data selection
eval $(awk -F'=' '/data_selection=[a-zA-Z0-9-_]*/{printf("DATA_SELECTION=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "/CONFIG_APP_TX_FRAME=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
sed -i "/[a-zA-Z0-9\"-\"_-]*DATA=y/d" $K_CONFIGFILE
if [ $DATA_SELECTION -eq 0 ];then
    echo "CONFIG_FIXED_DATA=y" >> $K_CONFIGFILE
    read APP_TX_FRAME < <(awk -F'=' '/tx_frame=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
    echo "CONFIG_APP_TX_FRAME=\"${APP_TX_FRAME%;*}\"" >> $K_CONFIGFILE
elif [ $DATA_SELECTION -eq 1 ];then
    echo "CONFIG_MKFIFO_DATA=y" >> $K_CONFIGFILE
elif [ $DATA_SELECTION -eq 2 ];then
    echo "CONFIG_HDC1080_DATA=y" >> $K_CONFIGFILE
elif [ $DATA_SELECTION -eq 3 ];then
    echo "CONFIG_BMP180_DATA=y" >> $K_CONFIGFILE
# elif [ $DATA_SELECTION -eq 4 ];then
fi


#Configure LoRaWAN debug 
eval $(awk -F'=' '/debug=[a-zA-Z0-9-_]*/{printf("DEBUG=%d\n") ,$2}' $INI_CONFIGFILE)
# echo $DEBUG
read DEBUG < <(awk -F'=' '/;PRINT_[a-zA-Z0-9-_]*='"${DEBUG}"'/{print $1}' $INI_CONFIGFILE)
# echo ${DEBUG: 7}
sed -i "s/CONFIG_PRINT_[a-zA-Z0-9_-]*=y/CONFIG_PRINT_${DEBUG: 7}=y/g" $K_CONFIGFILE


#Configure LoRaWAN channel 
eval $(awk -F'=' '/channel_start=[a-zA-Z0-9-_]*/{printf("START_CH_NUM=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "s/CONFIG_START_CH_NUM=[0-9]*/CONFIG_START_CH_NUM=${START_CH_NUM}/g" $K_CONFIGFILE
eval $(awk -F'=' '/channel_end=[a-zA-Z0-9-_]*/{printf("END_CH_NUM=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "s/CONFIG_END_CH_NUM=[0-9]*/CONFIG_END_CH_NUM=${END_CH_NUM}/g" $K_CONFIGFILE


#Configure LoRaWAN duty cycle 
eval $(awk -F'=' '/duty_cycle=[a-zA-Z0-9-_]*/{printf("APP_PORT=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "s/CONFIG_APP_TX_DUTY_CYCLE=[0-9]*/CONFIG_APP_TX_DUTY_CYCLE=${APP_PORT}/g" $K_CONFIGFILE

#Configure LoRaWAN app port
eval $(awk -F'=' '/app_port=[a-zA-Z0-9-_]*/{printf("APP_TX_DUTY_CYCLE=%d\n") ,$2}' $INI_CONFIGFILE)
sed -i "s/CONFIG_APP_PORT=[0-9]*/CONFIG_APP_PORT=${APP_TX_DUTY_CYCLE}/g" $K_CONFIGFILE


#Configure LoRaWAN adr
read ADR_ENABLE < <(awk -F'=' '/adr=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
# echo ${ADR_ENABLE:0:6}
if [  ${ADR_ENABLE:0:6} = 'enable' ];then
    read CONFIG_ADR_ENABLE < <(awk -F'=' '/^CONFIG_ADR_ENABLE=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_ADR_ENABLE ];then
         echo 11 >> /dev/null
    else
         echo "CONFIG_ADR_ENABLE=y" >> $K_CONFIGFILE
    fi
else
    read CONFIG_ADR_ENABLE < <(awk -F'=' '/^CONFIG_ADR_ENABLE=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_ADR_ENABLE ];then
        sed -i "/CONFIG_ADR_ENABLE=y/d" $K_CONFIGFILE
    else
        echo 11 >> /dev/null
    fi
fi


#Configure LoRaWAN tx confirmed
read TX_CONFIRMED < <(awk -F'=' '/tx_confirmed=[a-zA-Z0-9-_]*/{print $2}' $INI_CONFIGFILE)
# echo ${TX_CONFIRMED:0:6}
if [  ${TX_CONFIRMED:0:6} = 'enable' ];then
    read CONFIG_TX_CONFIRMED < <(awk -F'=' '/^CONFIG_TX_CONFIRMED=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_TX_CONFIRMED ];then
         echo 11 >> /dev/null
    else
         echo "CONFIG_TX_CONFIRMED=y" >> $K_CONFIGFILE
    fi
else
    read CONFIG_TX_CONFIRMED < <(awk -F'=' '/^CONFIG_TX_CONFIRMED=[a-zA-Z0-9-_]*/{print $1}' $K_CONFIGFILE)
    if [ $CONFIG_TX_CONFIRMED ];then
        sed -i "/CONFIG_TX_CONFIRMED=y/d" $K_CONFIGFILE
    else
        echo 11 >> /dev/null
    fi
fi


#Configure HelTec LoRaWAN license
read LICENSE < <(awk -F'=' '/license=[a-zA-Z0-9\"-\",-]*/{print $2}' $INI_CONFIGFILE)
sed -i "/CONFIG_HELTEC_LICENSE=[a-zA-Z0-9\"-\"_-]*/d" $K_CONFIGFILE
echo "CONFIG_HELTEC_LICENSE=\"${LICENSE%;*}\"" >> $K_CONFIGFILE