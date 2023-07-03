aux_source_directory(. GPIO)
#Set as a static library.
add_library(gpio ${GPIO})
#Set the current folder as the search directory for header files that depend on the gpio library.
target_include_directories(gpio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})