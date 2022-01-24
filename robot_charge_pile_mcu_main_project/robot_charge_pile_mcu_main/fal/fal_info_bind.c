#include "stm32f1xx_hal.h"
#include "fal_info_bind.h"
#include "pal_uros.h"
#include "log.h"
#include <flashdb.h>

#include <rcl/rcl.h>
#include <rclc/executor.h>

static service g_pile_set_dev_info;
chassis_interfaces__srv__PileSetDevInfo_Request pile_set_dev_info_req;
chassis_interfaces__srv__PileSetDevInfo_Response pile_set_dev_info_res;

static service g_pile_get_dev_info;
chassis_interfaces__srv__PileGetDevInfo_Response pile_get_dev_info_res;
chassis_interfaces__srv__PileGetDevInfo_Request pile_get_dev_info_req;

static service g_pile_set_wifi_info;
chassis_interfaces__srv__PileSetWifiInfo_Response pile_set_wifi_info_res;
chassis_interfaces__srv__PileSetWifiInfo_Request pile_set_wifi_info_req;

static service g_pile_get_wifi_info;
chassis_interfaces__srv__PileGetWifiInfo_Response pile_get_wifi_info_res;
chassis_interfaces__srv__PileGetWifiInfo_Request pile_get_wifi_info_req;

pile_dev_info_t kv_dev_info;
pile_wifi_info_t kv_wifi_info;
static struct fdb_blob blob;
extern struct fdb_kvdb kvdb;

void fal_set_dev_info(const void *req, void *res);
void fal_get_dev_info(const void *req, void *res);
void fal_set_wifi_info(const void *req, void *res);
void fal_get_wifi_info(const void *req, void *res);
void fal_info_bind_init(void) {
    service_init(
        &g_pile_set_dev_info,
        ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, PileSetDevInfo),
        "/auto_charge/pile_set_dev_info", &pile_set_dev_info_req,
        &pile_set_dev_info_res, BEST, fal_set_dev_info);

    service_init(
        &g_pile_get_dev_info,
        ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, PileGetDevInfo),
        "/auto_charge/pile_get_dev_info", &pile_get_dev_info_req,
        &pile_get_dev_info_res, BEST, fal_get_dev_info);

    service_init(
        &g_pile_set_wifi_info,
        ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, PileSetWifiInfo),
        "/auto_charge/pile_set_wifi_info", &pile_set_wifi_info_req,
        &pile_set_wifi_info_res, BEST, fal_set_wifi_info);

    service_init(
        &g_pile_get_wifi_info,
        ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, PileGetWifiInfo),
        "/auto_charge/pile_get_wifi_info", &pile_get_wifi_info_req,
        &pile_get_wifi_info_res, BEST, fal_get_wifi_info);

    MallocString(&pile_set_wifi_info_req.wifi.ssid, REQ, STRING_MAX);
    MallocString(&pile_set_wifi_info_req.wifi.key_mqmt, REQ, STRING_MAX);
    MallocString(&pile_set_wifi_info_req.wifi.psk, REQ, STRING_MAX);
    MallocString(&pile_set_wifi_info_req.wifi.bssid, REQ, STRING_MAX);
    MallocString(&pile_get_dev_info_res.hardware_id, RES, STRING_MAX);

    fdb_kv_get_blob(&kvdb, "kv_dev_info",
                    fdb_blob_make(&blob, &kv_dev_info, sizeof(kv_dev_info)));
    fdb_kv_get_blob(&kvdb, "kv_wifi_info",
                    fdb_blob_make(&blob, &kv_wifi_info, sizeof(kv_wifi_info)));
}

void fal_info_bind_deinit(void) {}

void fal_set_dev_info(const void *req, void *res) {
    chassis_interfaces__srv__PileSetDevInfo_Request *req_in =
        (chassis_interfaces__srv__PileSetDevInfo_Request *) req;
    chassis_interfaces__srv__PileSetDevInfo_Response *res_in =
        (chassis_interfaces__srv__PileSetDevInfo_Response *) res;

    kv_dev_info.stamp = req_in->header.stamp;

    kv_dev_info.iot_id[0] = (uint32_t)(req_in->iot_id >> 32);
    kv_dev_info.iot_id[1] = (uint32_t) req_in->iot_id;

    fdb_kv_set_blob(&kvdb, "kv_dev_info",
                    fdb_blob_make(&blob, &kv_dev_info, sizeof(kv_dev_info)));

    res_in->success = 1;

    LOG_DEBUG("sec:%ld,nanosec:%ld", req_in->header.stamp.sec,
              req_in->header.stamp.nanosec);
    LOG_DEBUG("iot_id:%ld", (uint32_t)(req_in->iot_id >> 32));
    LOG_DEBUG("iot_id:%ld", (uint32_t) req_in->iot_id);
    LOG_DEBUG("iot_id:%lld", req_in->iot_id);
}

void fal_get_dev_info(const void *req, void *res) {
    // chassis_interfaces__srv__PileGetDevInfo_Request * req_in =
    // (chassis_interfaces__srv__PileGetDevInfo_Request *) req;
    chassis_interfaces__srv__PileGetDevInfo_Response *res_in =
        (chassis_interfaces__srv__PileGetDevInfo_Response *) res;

    res_in->header.stamp = kv_dev_info.stamp;

    res_in->iot_id = (uint64_t) kv_dev_info.iot_id[1] +
                     ((uint64_t) kv_dev_info.iot_id[0] << 32);

    sprintf(res_in->hardware_id.data, "%08lx%08lx%08lx", HAL_GetUIDw0(),
            HAL_GetUIDw1(), HAL_GetUIDw2());

    res_in->model_name.data = MODEL_NAME;
    res_in->model_name.size = strlen(res_in->model_name.data);

    LOG_DEBUG("iot_id:%ld", kv_dev_info.iot_id[0]);
    LOG_DEBUG("iot_id:%ld", kv_dev_info.iot_id[1]);
    LOG_DEBUG("hardware_id:%s", res_in->hardware_id.data);
    LOG_DEBUG("model_name:%s", res_in->model_name.data);
}

void fal_set_wifi_info(const void *req, void *res) {
    chassis_interfaces__srv__PileSetWifiInfo_Request *req_in =
        (chassis_interfaces__srv__PileSetWifiInfo_Request *) req;
    chassis_interfaces__srv__PileSetWifiInfo_Response *res_in =
        (chassis_interfaces__srv__PileSetWifiInfo_Response *) res;

    kv_wifi_info.stamp = req_in->header.stamp;

    memset(kv_wifi_info.ssid, '\0', sizeof(kv_wifi_info.ssid));
    memcpy(kv_wifi_info.ssid, req_in->wifi.ssid.data, req_in->wifi.ssid.size);

    memset(kv_wifi_info.bssid, '\0', sizeof(kv_wifi_info.bssid));
    memcpy(kv_wifi_info.bssid, req_in->wifi.bssid.data,
           req_in->wifi.bssid.size);

    memset(kv_wifi_info.psk, '\0', sizeof(kv_wifi_info.psk));
    memcpy(kv_wifi_info.psk, req_in->wifi.psk.data, req_in->wifi.psk.size);

    kv_wifi_info.scan_ssid = req_in->wifi.scan_ssid;

    memset(kv_wifi_info.key_mqmt, '\0', sizeof(kv_wifi_info.key_mqmt));
    memcpy(kv_wifi_info.key_mqmt, req_in->wifi.key_mqmt.data,
           req_in->wifi.key_mqmt.size);

    fdb_kv_set_blob(&kvdb, "kv_wifi_info",
                    fdb_blob_make(&blob, &kv_wifi_info, sizeof(kv_wifi_info)));

    res_in->success = 1;

    LOG_DEBUG("sec:%ld,nanosec:%ld", req_in->header.stamp.sec,
              req_in->header.stamp.nanosec);
    LOG_DEBUG("ssid:%s", kv_wifi_info.ssid);
    LOG_DEBUG("bssid:%s", kv_wifi_info.bssid);
    LOG_DEBUG("psk:%s", kv_wifi_info.psk);
    LOG_DEBUG("scan_ssid:%d", kv_wifi_info.scan_ssid);
    LOG_DEBUG("key_mqmt:%s", kv_wifi_info.key_mqmt);
}

void fal_get_wifi_info(const void *req, void *res) {
    // chassis_interfaces__srv__PileGetWifiInfo_Request * req_in =
    // (chassis_interfaces__srv__PileGetWifiInfo_Request *) req;
    chassis_interfaces__srv__PileGetWifiInfo_Response *res_in =
        (chassis_interfaces__srv__PileGetWifiInfo_Response *) res;

    res_in->header.stamp = kv_wifi_info.stamp;

    res_in->wifi.ssid.data = kv_wifi_info.ssid;
    res_in->wifi.ssid.size = strlen(kv_wifi_info.ssid);

    res_in->wifi.bssid.data = kv_wifi_info.bssid;
    res_in->wifi.bssid.size = strlen(kv_wifi_info.bssid);

    res_in->wifi.psk.data = kv_wifi_info.psk;
    res_in->wifi.psk.size = strlen(kv_wifi_info.psk);

    res_in->wifi.scan_ssid = kv_wifi_info.scan_ssid;

    res_in->wifi.key_mqmt.data = kv_wifi_info.key_mqmt;
    res_in->wifi.key_mqmt.size = strlen(kv_wifi_info.key_mqmt);

    LOG_DEBUG("sec:%ld,nanosec:%ld", res_in->header.stamp.sec,
              res_in->header.stamp.nanosec);
    LOG_DEBUG("ssid:%s", res_in->wifi.ssid.data);
    LOG_DEBUG("bssid:%s", res_in->wifi.bssid.data);
    LOG_DEBUG("psk:%s", res_in->wifi.psk.data);
    LOG_DEBUG("scan_ssid:%d", res_in->wifi.scan_ssid);
    LOG_DEBUG("key_mqmt:%s", res_in->wifi.key_mqmt.data);
}