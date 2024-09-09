//
// Created by root on 9/9/24.
//

#ifndef OPCUA_SERVER_SDK_H
#define OPCUA_SERVER_SDK_H
#include "../opcua_server.h"
void init_server(neu_plugin_t *plugin);
void start_server(neu_plugin_t *plugin);
void stop_server(neu_plugin_t *plugin);
void uinit_server(neu_plugin_t *plugin);
void processData(neu_plugin_t *plugin, const char *jsonData);
#endif //OPCUA_SERVER_SDK_H
