#ifndef __FAL_OTA_H__
#define __FAL_OTA_H__

#include "stdio.h"
#include "rcl/rcl.h"
#include "pal_uros.h"
#include "std_msgs/msg/u_int8.h"
#include "chassis_interfaces/srv/upgrade.h"
#include "chassis_interfaces/srv/get_updating_info.h"
#include "chassis_interfaces/msg/upgrade_progess.h"
#include "chassis_interfaces/msg/firmware_info.h"

#define OTA_PACKSIZE 360
#define STRING_MAX   64

typedef struct {
    uint32_t ready_flag;
    uint32_t finish_flag;
} ota_t;

typedef struct {
    builtin_interfaces__msg__Time stamp;
    char                          version[STRING_MAX];
} firmware_info_t;

typedef struct {
    char     total_md5[STRING_MAX];
    char     buffered_md5[STRING_MAX];
    uint8_t  is_finished;
    uint32_t finish_time;
    uint32_t total_size;
    uint32_t buffered_size;
} updating_info_t;

updating_info_t main_updating_info;
firmware_info_t main_firmware_info;
updating_info_t motor_updating_info;
firmware_info_t motor_firmware_info;
updating_info_t usound_updating_info;
firmware_info_t usound_firmware_info;

void fal_ota_init(void);
void fal_ota_check_update(char *part_name, firmware_info_t *firmwareinfo, updating_info_t *updatinginfo, char *version);
#endif
