#Add source files.
aux_source_directory(. HDC1080)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_library(hdc1080 ${HDC1080} )

target_link_libraries(hdc1080 i2c )

target_include_directories(hdc1080 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})