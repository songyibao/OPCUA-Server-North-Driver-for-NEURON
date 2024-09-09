//
// Created by root on 9/9/24.
//

#include "utils.h"
#include "../sdk/sdk.h"
#include "json_rw.h"
int update_data(neu_plugin_t *plugin, neu_reqresp_head_t *head, neu_reqresp_trans_data_t *trans_data){
    int              ret      = 0;
    char            *json_str = NULL;
    json_read_resp_t resp     = {
            .plugin     = plugin,
            .trans_data = trans_data,
    };
    ret = neu_json_encode_by_fn(&resp, json_encode_read_resp, &json_str);
    if (ret != 0) {
        plog_notice(plugin, "parse json failed");
        return -1;
    }
    plog_debug(plugin, "parse json str succeed: %s", json_str);
    processData(plugin,json_str);
    free(json_str);
    return 0;
}