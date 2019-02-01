cmake_minimum_required(VERSION 2.8.11)

#set (CMAKE_C_FLAGS "--std=c99 -ggdb ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")

set(AZUREIOT_INC_FOLDER ".." "/usr/local/include/azureiot" "/usr/local/include/azureiot/inc")

include_directories(${AZUREIOT_INC_FOLDER})

set(sample_application_c_files
    ./sample.c
    ./cloud2device.c
    ./device2cloud.c
    ./devicetwin.c
    ./directmethod.c
    ./shared.c
    ./utils.c
)

set(sample_application_h_files
    ./shared.h
    ./cloud2device.h
    ./device2cloud.h
    ./devicetwin.h
    ./directmethod.h
    ./utils.h
)

add_executable(sample_app ${sample_application_c_files} ${sample_application_h_files})

target_link_libraries(sample_app
    serializer
    iothub_client
    iothub_client_amqp_transport
    iothub_client_mqtt_transport
    iothub_client_http_transport
    uamqp
    umqtt
    uhttp
    aziotsharedutil
    parson
    pthread
    curl
    ssl
    uuid
    crypto
    m
)