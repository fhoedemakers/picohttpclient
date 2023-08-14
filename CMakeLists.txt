cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

project(httpclient C CXX ASM)
pico_sdk_init()

add_executable(httpclient httpclient.c)
target_include_directories(httpclient PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(httpclient pico_stdlib  pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http)
pico_enable_stdio_usb(httpclient 1)
pico_enable_stdio_uart(httpclient 0)
pico_add_extra_outputs(httpclient)

