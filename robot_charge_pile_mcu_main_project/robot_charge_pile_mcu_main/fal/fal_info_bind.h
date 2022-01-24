#ifndef __FAL_INFO_BIND_H__
#define __FAL_INFO_BIND_H__

#include "chassis_interfaces/srv/pile_set_dev_info.h"
#include "chassis_interfaces/srv/pile_get_dev_info.h"
#include "chassis_interfaces/srv/pile_set_wifi_info.h"
#include "chassis_interfaces/srv/pile_get_wifi_info.h"
#include "chassis_interfaces/msg/wifi_info.h"
#include "stdio.h"

#define STRING_MAX 64

typedef struct {
    builtin_interfaces__msg__Time stamp;
    uint32_t iot_id[2];
} pile_dev_info_t;

typedef struct {
    builtin_interfaces__msg__Time stamp;
    char ssid[STRING_MAX];
    char bssid[STRING_MAX];
    char psk[STRING_MAX];
    uint8_t scan_ssid;
    char key_mqmt[STRING_MAX];
} pile_wifi_info_t;

void fal_info_bind_init(void);

void fal_set_dev_info(const void *req, void *res);
void fal_get_dev_info(const void *req, void *res);
void fal_set_wifi_info(const void *req, void *res);
void fal_get_wifi_info(const void *req, void *res);

#endif