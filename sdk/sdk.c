//
// Created by root on 9/9/24.
//
#include "sdk.h"
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <string.h>
#include <yyjson.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../opcua_server.h"



// Function to find or create a folder node
UA_NodeId findOrCreateFolderNode(neu_plugin_t *plugin, const UA_NodeId parentNodeId, const char *folderName) {
    UA_Server *server = plugin->server;
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, (char*)folderName);

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = parentNodeId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr = UA_Server_translateBrowsePathToNodeIds(server, &bp);

    if(bpr.statusCode == UA_STATUSCODE_GOOD && bpr.targetsSize > 0) {
        UA_NodeId result = bpr.targets[0].targetId.nodeId;
        UA_BrowsePathResult_clear(&bpr);
        return result;
    }

    UA_BrowsePathResult_clear(&bpr);

    // If not found, create a new folder node
    UA_NodeId folderNodeId;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)folderName);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
                            parentNodeId,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, (char*)folderName),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
                            oAttr, NULL, &folderNodeId);
    return folderNodeId;
}

// Function to find or create a variable node
UA_NodeId findOrCreateVariableNode(neu_plugin_t *plugin, const UA_NodeId parentNodeId, const char *variableName) {
    UA_Server *server = plugin->server;
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, (char*)variableName);

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = parentNodeId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr = UA_Server_translateBrowsePathToNodeIds(server, &bp);

    if(bpr.statusCode == UA_STATUSCODE_GOOD && bpr.targetsSize > 0) {
        UA_NodeId result = bpr.targets[0].targetId.nodeId;
        UA_BrowsePathResult_clear(&bpr);
        return result;
    }

    UA_BrowsePathResult_clear(&bpr);

    // If not found, create a new variable node
    UA_NodeId variableNodeId;
    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)variableName);
    vAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Server_addVariableNode(server, UA_NODEID_NULL,
                              parentNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, (char*)variableName),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                              vAttr, NULL, &variableNodeId);
    return variableNodeId;
}

// Function to update node value
void updateNodeValue(neu_plugin_t *plugin, UA_NodeId nodeId, double value) {
    UA_Server *server = plugin->server;
    UA_Variant variant;
    UA_Variant_setScalar(&variant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, nodeId, variant);
}

// Function to process incoming data
void processData(neu_plugin_t *plugin, const char *jsonData) {
    UA_Server *server = plugin->server;
    yyjson_doc *doc = yyjson_read(jsonData, strlen(jsonData), 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    const char *nodeName = yyjson_get_str(yyjson_obj_get(root, "node_name"));
    const char *groupName = yyjson_get_str(yyjson_obj_get(root, "group_name"));
    yyjson_val *values = yyjson_obj_get(root, "values");

    // Find or create group folder
    UA_NodeId groupNodeId = findOrCreateFolderNode(plugin, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), nodeName);

    // Find or create device folder
    UA_NodeId deviceNodeId = findOrCreateFolderNode(plugin, groupNodeId, groupName);

    // Process values
    yyjson_val *key, *val;
    yyjson_obj_iter iter;
    yyjson_obj_iter_init(values, &iter);
    while ((key = yyjson_obj_iter_next(&iter))) {
        val = yyjson_obj_iter_get_val(key);
        const char *keyStr = yyjson_get_str(key);
        double valNum = yyjson_get_num(val);

        UA_NodeId variableNodeId = findOrCreateVariableNode(plugin, deviceNodeId, keyStr);
        updateNodeValue(plugin, variableNodeId, valNum);
    }

    yyjson_doc_free(doc);
}

// Thread function for running the OPC UA server
void* serverThread(void* arg) {
    neu_plugin_t *plugin = (neu_plugin_t *)arg;
    UA_Server_run_startup(plugin->server);
    while (plugin->running == true) {
        plugin->common.link_state = NEU_NODE_LINK_STATE_CONNECTED;
        UA_Server_run_iterate(plugin->server, true);
    }
    UA_Server_run_shutdown(plugin->server);
    return NULL;
}

void init_server(neu_plugin_t *plugin){
    plog_info(plugin,"Init opcua server");
    plugin->server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(plugin->server));
}
void start_server(neu_plugin_t *plugin){
    plog_info(plugin,"Starting opcua server");
    plugin->running = true;
    pthread_create(&plugin->server_thread, NULL, serverThread, plugin);
}
void stop_server(neu_plugin_t *plugin){
    plog_info(plugin,"Stopping opcua server");
    plugin->running = false;
    plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;
    pthread_join(plugin->server_thread, NULL);

}
void uinit_server(neu_plugin_t *plugin){
    plog_info(plugin,"Uinit opcua server");
    UA_Server_delete(plugin->server);
}