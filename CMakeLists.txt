cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

# Assume WIFI_SSID, WIFI_PASSWORD, WIFI_COUNTRY, REMOTE_DOMAIN and RELATIVE_URL are defined in $ENV{HOME}/wifiparams.cmake
set(HOME_FILE_PATH "$ENV{HOME}/wifiparams.cmake")
if (EXISTS ${HOME_FILE_PATH})
    include(${HOME_FILE_PATH})
else()
    message(FATAL_ERROR "Please create $ENV{HOME}/wifiparams.cmake with the following content:\nset(WIFI_SSID \"your ssid\")\nset(WIFI_PASSWORD \"your password\")\nset(WIFI_COUNTRY \"your wifi country\")\nset(REMOTE_DOMAIN \"dns name of remote site\")\nset(RELATIVE_URL \"relative url of webpage/file\")\n")
endif()

add_compile_definitions(WIFI_SSID="${WIFI_SSID}")
add_compile_definitions(WIFI_PASSWORD="${WIFI_PASSWORD}")
add_compile_definitions(WIFI_COUNTRY="${WIFI_COUNTRY}")
add_compile_definitions(REMOTE_DOMAIN="${REMOTE_DOMAIN}")
add_compile_definitions(RELATIVE_URL="${RELATIVE_URL}")

project(httpclient C CXX ASM)
pico_sdk_init()

add_executable(httpclient httpclient.c)
target_include_directories(httpclient PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(httpclient pico_stdlib  pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http)
pico_enable_stdio_usb(httpclient 1)
pico_enable_stdio_uart(httpclient 0)
pico_add_extra_outputs(httpclient)

