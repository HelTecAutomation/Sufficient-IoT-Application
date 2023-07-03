#The smallest version of cmake.
cmake_minimum_required(VERSION 3.0)
project(demo)

#Set the flag of the compiler.
set(CXX_FLAGS
 -g
 )

#Set the output directory of the executable file.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#Set the output directory of the library file.
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#Add the project folder under the current folder as a subfolder, enter the folder and execute the CMakeLists.txt file in its directory.
add_subdirectory(${PROJECT_SOURCE_DIR}/hardware_driver)
add_subdirectory(${PROJECT_SOURCE_DIR}/functional_module)
add_subdirectory(${PROJECT_SOURCE_DIR}/project)

#make menuconfig executes the previous execution command.
add_custom_target( 
  menuconfig
  COMMAND  bash ${PROJECT_SOURCE_DIR}/project/config/kconfig_lib/configini_to_kconfig.sh
  COMMAND  python3 ${PROJECT_SOURCE_DIR}/project/config/kconfig_lib/menuconfig.py ${PROJECT_SOURCE_DIR}/Kconfig 
  USES_TERMINAL
)

#The command to be executed after make menuconfig is executed. 
add_custom_command( 
  TARGET menuconfig
  POST_BUILD
  COMMAND bash ${PROJECT_SOURCE_DIR}/project/config/kconfig_lib/kconfig_to_configini.sh
)