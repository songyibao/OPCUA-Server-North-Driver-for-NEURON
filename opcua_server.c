//
// Created by songyibao on 24-2-29.
//

#include "opcua_server.h"
#include "neuron.h"
#include <pthread.h>
#include <stdlib.h>
#include "sdk/sdk.h"
#include "utils/utils.h"
#define DESCRIPTION "OPCUA Server North Driver"
#define DESCRIPTION_ZH "OPCUA Server 北向插件"
static const neu_plugin_intf_funs_t plugin_intf_funs = {
        .open    = driver_open,
        .close   = driver_close,
        .init    = driver_init,
        .uninit  = driver_uninit,
        .start   = driver_start,
        .stop    = driver_stop,
        .setting = driver_config,
        .request = driver_request,

        .driver.validate_tag = driver_validate_tag,
        .driver.group_timer  = driver_group_timer,
        .driver.write_tag    = driver_write,
};

const neu_plugin_module_t neu_plugin_module = {
        .version         = NEURON_PLUGIN_VER_1_0,
        .schema          = "opcua-server",
        .module_name     = "OPCUA Server",
        .module_descr    = DESCRIPTION,
        .module_descr_zh = DESCRIPTION_ZH,
        .intf_funs       = &plugin_intf_funs,
        .kind            = NEU_PLUGIN_KIND_SYSTEM,
        .type            = NEU_NA_TYPE_APP,
        .display         = true,
        .single          = false,
};

static neu_plugin_t *driver_open(void)
{

    neu_plugin_t *plugin = calloc(1, sizeof(neu_plugin_t));

    neu_plugin_common_init(&plugin->common);
    plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;

    return plugin;
}

static int driver_close(neu_plugin_t *plugin)
{
    free(plugin);

    return 0;
}
// driver_init -> driver_config -> driver_start
static int driver_init(neu_plugin_t *plugin, bool load)
{
    (void) load;
    plog_notice(plugin,
                "============================================================"
                "\ninitialize "
                "plugin============================================================\n");
    plugin->server = NULL;
    init_server(plugin);
    return 0;
}

static int driver_config(neu_plugin_t *plugin, const char *setting)
{
    plog_notice(plugin,
                "============================================================\nconfig "
                "plugin============================================================\n");
    int res = 0;

    return res;
}

static int driver_start(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nstart "
                "plugin============================================================\n");


    start_server(plugin);
    plugin->started          = true;
    return 0;
}

static int driver_stop(neu_plugin_t *plugin)
{

    plog_notice(plugin,
                "============================================================\nstop "
                "plugin============================================================\n");
    plugin->started = false;
    stop_server(plugin);
    return 0;
}

static int driver_uninit(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nuninit "
                "plugin============================================================\n");
    uinit_server(plugin);
    nlog_debug("uninit success");
    return NEU_ERR_SUCCESS;
}

static int driver_request(neu_plugin_t *plugin, neu_reqresp_head_t *head, void *data)
{
    plog_notice(plugin,
                "============================================================request "
                "plugin============================================================\n");
    neu_reqresp_trans_data_t *trans_data = (neu_reqresp_trans_data_t *)data;
    if(plugin->started == false){
        plog_info(plugin, "插件未启动");
        return NEU_ERR_PLUGIN_NOT_RUNNING;
    }
    if (plugin->running == false) {
        plog_error(plugin, "OPCUA_Server 未启动");
        return NEU_ERR_PLUGIN_DISCONNECTED;
    }


    neu_err_code_e error = NEU_ERR_SUCCESS;
    switch (head->type) {
        case NEU_REQ_SUBSCRIBE_GROUP:
            break;
        case NEU_REQRESP_TRANS_DATA: {
            update_data(plugin,head,trans_data);
            break;
        }
        default:
            break;
    }
    plog_notice(plugin, "Exit request function");
    return error;
}

static int driver_validate_tag(neu_plugin_t *plugin, neu_datatag_t *tag)
{
    plog_notice(plugin, "validate tag: %s", tag->name);

    return 0;
}

static int driver_group_timer(neu_plugin_t *plugin, neu_plugin_group_t *group)
{
    (void) plugin;
    (void) group;

    plog_notice(plugin, "timer....");

    return 0;
}

static int driver_write(neu_plugin_t *plugin, void *req, neu_datatag_t *tag, neu_value_u value)
{
    (void) plugin;
    (void) req;
    (void) tag;
    (void) value;

    return 0;
}