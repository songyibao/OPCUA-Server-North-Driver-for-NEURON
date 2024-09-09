//
// Created by root on 9/9/24.
//
#include "config_parse.h"

#include "../opcua_server.h"
// 连接两个指针字符串，返回一个新开辟的内存区域，不会修改原来的字符串
char *concatenate(const char *prefix, const char *suffix)
{
    char *result = malloc(strlen(prefix) + strlen(suffix) + 1); // Allocate memory for the concatenated string
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, prefix); // Copy prefix to result
    strcpy(result + strlen(prefix), suffix);
    return result;
}
int config_parse(neu_plugin_t *plugin, const char *setting)
{
    int   ret       = 0;
    char *err_param = NULL;

    neu_json_elem_t qos             = { .name = "qos", .t = NEU_JSON_INT };
    neu_json_elem_t host            = { .name = "host", .t = NEU_JSON_STR };
    neu_json_elem_t port            = { .name = "port", .t = NEU_JSON_INT };
    neu_json_elem_t deviceid        = { .name = "deviceid", .t = NEU_JSON_STR };
    neu_json_elem_t userid          = { .name = "userid", .t = NEU_JSON_STR };
    neu_json_elem_t productid       = { .name = "productid", .t = NEU_JSON_STR };
    neu_json_elem_t firmwareversion = { .name = "firmwareversion", .t = NEU_JSON_STR };
    neu_json_elem_t interval        = { .name = "interval", .t = NEU_JSON_INT };
    ret = neu_parse_param(setting, &err_param, 8, &qos, &host, &port, &deviceid, &userid, &productid, &firmwareversion,
                          &interval);
    if (ret < 0) {
        plog_error(plugin, "parse setting failed: %s", err_param);
        goto error;
    }
    // host, required
    if (0 == strlen(host.v.val_str)) {
        plog_error(plugin, "setting invalid host: `%s`", host.v.val_str);
        goto error;
    }

    // port, required
    if (0 == port.v.val_int || port.v.val_int > 65535) {
        plog_error(plugin, "setting invalid port: %" PRIi64, port.v.val_int);
        goto error;
    }

    // qos, required
    if (qos.v.val_int < 0 || qos.v.val_int > 2) {
        plog_error(plugin, "setting invalid port: %" PRIi64, qos.v.val_int);
        goto error;
    }

//    switch (qos.v.val_int) {
//        case 0:
//            plugin->qos = 0;
//            break;
//        case 1:
//            plugin->qos = 1;
//            break;
//        case 2:
//            plugin->qos = 2;
//            break;
//        default:
//            break;
//    }
//
//    plugin->host = host.v.val_str;
//    plugin->port = (uint16_t) port.v.val_int;
//    //    plugin->url  = url.v.val_str;
//    char port_str[6];
//    sprintf(port_str, "%ld", port.v.val_int);
//
//    // 生成 url
//    char *template = "mqtt-quic://%s:%s";
//    plugin->url    = (char *) malloc(strlen(template) + strlen(host.v.val_str) + strlen(port_str) + 1);
//    sprintf(plugin->url, template, host.v.val_str, port_str);
//
//    // NEURON 框架貌似不会自动释放val_str类型，不知道别的地方会不会再次用到这个指针，所以先保留
//    //    free(url.v.val_str);
//
//    plog_notice(plugin, "config url            : %s", plugin->url);
//    plog_notice(plugin, "config qos            : %d", plugin->qos);

    return 0;

    error:
    return -1;
}
int config_uint(neu_plugin_t *plugin){
//    free(plugin->url);
}