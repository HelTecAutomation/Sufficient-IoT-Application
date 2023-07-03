aux_source_directory(. I2C)
#Set as a static library.
add_library(i2c  ${I2C})
#Set the current folder as the search directory for header files that depend on the i2c library.
target_include_directories(i2c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})