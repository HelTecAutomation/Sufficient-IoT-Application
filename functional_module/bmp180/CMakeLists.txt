#Add source files.
aux_source_directory(. BMP180)

link_directories(${PROJECT_SOURCE_DIR}/lib)

LINK_LIBRARIES(-lm)
add_library(bmp180 ${BMP180} )

target_link_libraries(bmp180 i2c )

target_include_directories(bmp180 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})