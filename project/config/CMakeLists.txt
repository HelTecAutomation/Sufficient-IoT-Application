aux_source_directory(. CONFIG)
#Generate link library.
add_library(config ${CONFIG})
#Set the current folder as the search directory for header files that depend on the config library.
target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})