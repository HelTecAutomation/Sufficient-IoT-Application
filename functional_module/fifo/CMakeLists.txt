aux_source_directory(. FIFO)
#Set as a static library.
add_library(fifo ${FIFO})
#Set the current folder as the search directory for header files that depend on the fifo library.
target_include_directories(fifo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})