link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} fifo config lora hdc1080 bmp180 data_conversion)