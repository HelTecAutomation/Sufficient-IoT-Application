# Linux_LoRaWAN

English |[中文](README_ZH.md)    
![构建状态](https://img.shields.io/badge/build-passing-green)
![版本](https://img.shields.io/badge/HelTec__LoRaWAN-1.0-green)<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-2-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

[toc]

---------------------------------------

## Introduction

> - Implementation of **LoRaWAN** application under Linux, supporting LoRaWAN 1.0.2 protocolv
> - Supports multiple frequency bands
> - Build with **cmake** and easily add your own application code
> - Support **graphical interface** for parameter configuration

---------------------------------------

## install

1. First, you need to install make, camke, and gcc.
2. Download the source code locally.
3. Enter the build folder under the project and execute the following commands in sequence.

```sh
rm -rf *
cmake ..
make
```

4. After the execution is completed, an executable file will be generated in the bin folder.

## use

1. After the execution is completed, an executable file will be generated in the bin folder.

```sh
make menuconfig
```

2. Enter the bin folder of the project and execute the startup command.

```sh
./demo
```

3. If you encounter **Certificate verification failed**, please refer to License Verification.

## License Validation

1. Enter the folder of LoRaWAN executable files.

```sh
cd /home/lora/linux_lora/bin/
```

2. Execution ./demo

```c
Certificate verification failed
Please provide a correct license! For more information:
http://www.heltec.cn/search/ 
ChipID=303030303337   
```

3. Open the [address](https://resource.heltec.cn/search) connection to obtain the license, enter the **ChipID** of the product obtained in the previous step, and you can obtain the required license.
4. Use the shell command **make menuconfig** to open the configuration interface, find the license input option, and enter the aforementioned license. Save and exit.

```sh
HelTec LoRaWAN license --->
```

---------------------------------------

## Basic structure of the project

```c
|-- bin
|-- build
|-- functional_module
|   |-- bmp180
|   |-- data_conversion
|   |-- fifo
|   |-- hdc1080
|   `-- lora
|-- hardware_driver
|   |-- gpio
|   |-- i2c
|   `-- spi
|-- lib
`-- project
    |-- config
    `-- main
```

---------------------------------------

##  Currently supporting frequency bands

 >- [x] AS923 region
 >- [x] AU915 region
 >- [x] CN470 region
 >- [ ] CN779 region
 >- [ ] EU433 region
 >- [x] EU868 region
 >- [ ] KR920 region
 >- [ ] IN865 region
 >- [x] US915 region
 >- [ ] US915_HYBRID region
 >- [ ] AU915 SB2 region
 >- [x] AS923 AS1 region
 >- [ ] AS923 AS2 region

---------------------------------------

## Add your own functional modules

**Taking bmp180 as an example**

1. Add a new folder named bmp180 under the functional_module folder.

2. Add the following file in the bmp180 folder

   > bmp180.c
   >
   > bmp180.h
   >
   > CMakeLists.txt
   >
   > README.md

3. Write source file and Header file content.
4. Write CMakeList.txt

```cm
aux_source_directory(. BMP180)
include_directories(${PROJECT_SOURCE_DIR}/hardware_driver/i2c)
link_directories(${PROJECT_SOURCE_DIR}/lib)
LINK_LIBRARIES(-lm)
add_library(bmp180 ${BMP180} )
target_link_libraries(bmp180 i2c )
target_include_directories(bmp180 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

5. Write a `README.md` instruction document.

6. add in functional_module/CMakeLists.txt

   ```cm
   add_subdirectory(bmp180)
   ```

7. Add bmp180 in the link library option of project/main/CMakeLists.txt

   ```cm
   target_link_libraries(${PROJECT_NAME} fifo config lora hdc1080 bmp180 data_conversion)
   ```

8. Add your own Header file of bmp180 in main. c to call the bmp180 function.

--------------------------------------



## Add graphical configuration interface options

1. Find the Kconfig file for the project and add it in the LORAWAN_APP_DATA menu bar

   ```sh
   config BMP180_DATA
       bool "bmp180"
   ```

2. Add to the LORAWAN_DATA_SELECTION menu bar in project/config/config.ini

   ```in
   ;BMP180_DATA=3
   ```

   - The name of this option needs to be consistent with the options in Kconfig, both being **BMP180_DATA**.

   - The value of this option needs to be matched with the option to prepare data in main.c, and its values are all equal to **3**

     ```c
     typedef enum 
     {
         fixed_data =0,
         mkfifo_data,
         hdc1080_data,
         bmp180_data
     }data_selection_num;
     ```

3. In project/config/kconfig_lib/configini_to_kconfig.sh's #Configure LoRaWAN upload data selection add

   ```sh
   elif [ $DATA_SELECTION -eq 3 ];then
       echo "CONFIG_BMP180_DATA=y" >> $K_CONFIGFILE
   ```

   - configini_to_kconfig.sh will synchronize the C configuration file with the graphical interface configuration file.

4. The bmp180 configuration option added this time does not require modifying the project/config/kconfig_lib/kconfig_to_configini.sh file, as the previous synchronization options have already matched bmp180.
   - kconfig_to_configini.sh will synchronize the graphical interface configuration file to the C configuration file.

5. Add and read the config.ini file in project/main/main.c .

   ```c
    data_selection = iniparser_getint(ini,"LORAWAN_DATA_SELECTION:data_selection",-1);
   ```

6. Use make menuconfig to configure the bmp180 option and save it. At this point, the data_selection=3 is read in main.c.
7. After adding, you can implement relevant functions through options in man.c.

---------------------------------------

## Maintainer

[@Quency-D](https://github.com/Quency-D)

---------------------------------------

## contribute

Thank you very much for joining! Raise an Issue or submit a Pull Request.

The standard Readme follows the [Contributor Covenant](http://contributor-covenant.org/version/1/3/0/) code of conduct.

---------------------------------------

## contributor

Thank you to the following people who participated in the project:

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->

<table>
  <tr>
    <td align="center"><a href="https://github.com/Luckybuyu"><img src="https://avatars.githubusercontent.com/u/53414078?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Luckybuyu</b></sub></a><br /><a href="#design-Luckybuyu" title="Design">🎨</a></td>
    <td align="center"><a href="https://heltec.org"><img src="https://avatars.githubusercontent.com/u/32860565?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Aaron.Lee</b></sub></a><br /><a href="https://github.com/Quency-D/linux_lora/commits?author=Heltec-Aaron-Lee" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->



---------------------------------------

## license

[Apache License 2.0](LICENSE)



