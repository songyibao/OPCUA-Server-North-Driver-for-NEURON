cmake_minimum_required(VERSION 3.28)
set(LIBRARY_OUTPUT_PATH "./")
file(COPY /software/neuron/plugins/opcua_server/opcua-server.json DESTINATION
        /software/neuron/build/plugins/schema)
set(PROJECT_NAME plugin-opcua-server)
project(${PROJECT_NAME})

# Call this from your own project's makefile.

add_library(${PROJECT_NAME} SHARED opcua_server.c config/config_parse.c
        sdk/sdk.c
        utils/utils.c
        utils/json_rw.c)

target_include_directories(${PROJECT_NAME} PRIVATE
        /software/neuron/include/neuron
)

target_link_libraries(${PROJECT_NAME} PRIVATE /usr/local/lib/neuron/libneuron-base.so yyjson /usr/local/lib/libopen62541.a)
