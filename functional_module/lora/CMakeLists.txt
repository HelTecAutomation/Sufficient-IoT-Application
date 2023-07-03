#Add subfolders.
#add_subdirectory(time)
#Add source files.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/loramac LORAMAC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/loramac/region LORAMAC_REGION)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/radio RADIO)
aux_source_directory(. LORA)

#Add header files.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/loramac ${CMAKE_CURRENT_SOURCE_DIR}/loramac/Region ${CMAKE_CURRENT_SOURCE_DIR}/radio .)

#Connect to external libraries.
LINK_LIBRARIES(-lpthread -lm)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_library(lora ${LORAMAC} ${LORAMAC_REGION} ${RADIO} ${LORA})

target_link_libraries(lora lora_timer spi gpio )
#Set the current folder as the search directory for header files that depend on the fifo library.
target_include_directories(lora PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/loramac ${CMAKE_CURRENT_SOURCE_DIR}/loramac/region ${CMAKE_CURRENT_SOURCE_DIR}/radio ${CMAKE_CURRENT_SOURCE_DIR}/time )
