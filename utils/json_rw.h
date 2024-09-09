//
// Created by root on 9/9/24.
//

#ifndef PLUGIN_OPCUA_SERVER_JSON_RW_H
#define PLUGIN_OPCUA_SERVER_JSON_RW_H
#include <sys/time.h>

#include "neuron.h"
#include "json/neu_json_rw.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: these utilities may better be reused by mqtt and restful plugin
//       should move to neuron public sources

typedef struct {
    char *   node_name;
    char *   group_name;
    uint64_t timestamp;
} json_read_resp_header_t;

int wrap_tag_data(neu_json_read_resp_tag_t * json_tag,
                  neu_resp_tag_value_meta_t *tag_value);

typedef struct {
    neu_plugin_t *            plugin;
    neu_reqresp_trans_data_t *trans_data;
} json_read_resp_t;

// { "node_name": "node0", "group_name": "grp0", "timestamp": 1649776722631 }
int json_encode_read_resp_header(void *json_object, void *param);

// { "values": { "tag0": 0 }, "errors": { "tag1": 3000 } }
int json_encode_read_resp_tags(void *json_object, void *param);

// {
//    "node_name": "node0",
//    "group_name": "grp0",
//    "timestamp": 1649776722631,
//    "values": { "tag0": 0 },
//    "errors": { "tag1": 3000 }
// }
int json_encode_read_resp(void *json_object, void *param);

typedef struct {
    char *               node_name;
    char *               group_name;
    char *               tag_name;
    enum neu_json_type   t;
    union neu_json_value value;
} json_write_req_t;

int  json_decode_write_req(char *buf, size_t len, json_write_req_t **result);
void json_decode_write_req_free(json_write_req_t *req);

#ifdef __cplusplus
}
#endif
#endif //PLUGIN_OPCUA_SERVER_JSON_RW_H
