#All path variables are not allowed to have the same name.
aux_source_directory(. SPI)
#Set as a static library.
add_library(spi  ${SPI})
#Set the current folder as the search directory for header files that depend on the spi library.
target_include_directories(spi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})