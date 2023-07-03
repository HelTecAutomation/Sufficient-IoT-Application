aux_source_directory(. DATA_CONVERSION)
#Set as a static library.
add_library(data_conversion ${DATA_CONVERSION})
#Set the current folder as the search directory for header files that depend on the data_conversion library.
target_include_directories(data_conversion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})