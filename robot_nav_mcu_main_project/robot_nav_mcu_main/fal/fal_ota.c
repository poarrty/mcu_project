#include "fal_ota.h"
#include "md5.h"
#include "string.h"
#include "cmsis_os.h"
#include "queue.h"
#include "rtt_fal.h"
#include "flashdb.h"
#include "shell.h"
#include "fal_usound.h"
#define LOG_TAG "fal_ota"
#include "elog.h"

uint8_t                file_temp_buff[OTA_PACKSIZE] = {0};
static struct fdb_blob blob;
extern struct fdb_kvdb kvdb;

static subscrption          g_discover;
static std_msgs__msg__UInt8 discover;

static void fal_ota_discovered(const void *msgin);

static publisher                             g_firmware_info;
static chassis_interfaces__msg__FirmwareInfo firmware_info;
/*主MCU升级*/
#define MAIN_APP_KEY "65a5d9b99b0f29a0a8a72db6e3ecfbd7331078b3"

static service                                   g_main_upgrade;
static chassis_interfaces__srv__Upgrade_Request  main_upgrade_req;
static chassis_interfaces__srv__Upgrade_Response main_upgrade_res;

static service                                           g_main_get_updating_info;
static chassis_interfaces__srv__GetUpdatingInfo_Request  main_get_updating_info_req;
static chassis_interfaces__srv__GetUpdatingInfo_Response main_get_updating_info_res;

static publisher                               g_main_upgrade_progress;
static chassis_interfaces__msg__UpgradeProgess main_upgrade_progress;

static void main_upgrade(const void *req, void *res);
static void main_get_updating_info(const void *req, void *res);

updating_info_t main_updating_info = {0};
firmware_info_t main_firmware_info = {0};
/*驱动器MCU升级*/
#define MOTOR_APP_KEY "e0a9046eacb56737195a4f36807dc97669b6cec9"

static service                                   g_motor_upgrade;
static chassis_interfaces__srv__Upgrade_Request  motor_upgrade_req;
static chassis_interfaces__srv__Upgrade_Response motor_upgrade_res;

static service                                           g_motor_get_updating_info;
static chassis_interfaces__srv__GetUpdatingInfo_Request  motor_get_updating_info_req;
static chassis_interfaces__srv__GetUpdatingInfo_Response motor_get_updating_info_res;

static publisher                               g_motor_upgrade_progress;
static chassis_interfaces__msg__UpgradeProgess motor_upgrade_progress;

static void motor_upgrade(const void *req, void *res);
static void motor_get_updating_info(const void *req, void *res);

updating_info_t motor_updating_info = {0};
firmware_info_t motor_firmware_info = {0};
/*超声MCU升级*/
#define USOUND_APP_KEY "33ff032e15bd02372094e9021858e1c713c7915f"

static service                                   g_usound_upgrade;
static chassis_interfaces__srv__Upgrade_Request  usound_upgrade_req;
static chassis_interfaces__srv__Upgrade_Response usound_upgrade_res;

static service                                           g_usound_get_updating_info;
static chassis_interfaces__srv__GetUpdatingInfo_Request  usound_get_updating_info_req;
static chassis_interfaces__srv__GetUpdatingInfo_Response usound_get_updating_info_res;

static publisher                               g_usound_upgrade_progress;
static chassis_interfaces__msg__UpgradeProgess usound_upgrade_progress;

static void usound_upgrade(const void *req, void *res);
static void usound_get_updating_info(const void *req, void *res);

updating_info_t usound_updating_info = {0};
firmware_info_t usound_firmware_info = {0};

void fal_ota_init(void) {
    subscrption_init(&g_discover, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/ota/discover", &discover, BEST, fal_ota_discovered);

    publisher_init(&g_firmware_info, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, FirmwareInfo), "/ota/firmware_info", &firmware_info,
                   BEST, OVERWRITE, sizeof(chassis_interfaces__msg__FirmwareInfo));

    /*主MCU升级*/
    publisher_init(&g_main_upgrade_progress, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, UpgradeProgess),
                   "/ota_" MAIN_APP_KEY "/upgrade_progress", &main_upgrade_progress, BEST, OVERWRITE,
                   sizeof(chassis_interfaces__msg__UpgradeProgess));

    service_init(&g_main_upgrade, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, Upgrade), "/ota_" MAIN_APP_KEY "/upgrade",
                 &main_upgrade_req, &main_upgrade_res, BEST, main_upgrade);

    service_init(&g_main_get_updating_info, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, GetUpdatingInfo),
                 "/ota_" MAIN_APP_KEY "/get_updating_info", &main_get_updating_info_req, &main_get_updating_info_res, BEST,
                 main_get_updating_info);

    MallocString(&main_upgrade_req.package_md5, REQ, STRING_MAX);
    MallocString(&main_upgrade_req.total_md5, REQ, STRING_MAX);
    main_upgrade_req.package_data.data     = file_temp_buff;
    main_upgrade_req.package_data.size     = 0;
    main_upgrade_req.package_data.capacity = sizeof(file_temp_buff);
    /*驱动器MCU升级*/
    publisher_init(&g_motor_upgrade_progress, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, UpgradeProgess),
                   "/ota_" MOTOR_APP_KEY "/upgrade_progress", &motor_upgrade_progress, BEST, OVERWRITE,
                   sizeof(chassis_interfaces__msg__UpgradeProgess));

    service_init(&g_motor_upgrade, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, Upgrade), "/ota_" MOTOR_APP_KEY "/upgrade",
                 &motor_upgrade_req, &motor_upgrade_res, BEST, motor_upgrade);

    service_init(&g_motor_get_updating_info, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, GetUpdatingInfo),
                 "/ota_" MOTOR_APP_KEY "/get_updating_info", &motor_get_updating_info_req, &motor_get_updating_info_res, BEST,
                 motor_get_updating_info);

    MallocString(&motor_upgrade_req.package_md5, REQ, STRING_MAX);
    MallocString(&motor_upgrade_req.total_md5, REQ, STRING_MAX);
    motor_upgrade_req.package_data.data     = file_temp_buff;
    motor_upgrade_req.package_data.size     = 0;
    motor_upgrade_req.package_data.capacity = sizeof(file_temp_buff);
    /*超声MCU升级*/
    publisher_init(&g_usound_upgrade_progress, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, UpgradeProgess),
                   "/ota_" USOUND_APP_KEY "/upgrade_progress", &usound_upgrade_progress, BEST, OVERWRITE,
                   sizeof(chassis_interfaces__msg__UpgradeProgess));

    service_init(&g_usound_upgrade, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, Upgrade), "/ota_" USOUND_APP_KEY "/upgrade",
                 &usound_upgrade_req, &usound_upgrade_res, BEST, usound_upgrade);

    service_init(&g_usound_get_updating_info, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, GetUpdatingInfo),
                 "/ota_" USOUND_APP_KEY "/get_updating_info", &usound_get_updating_info_req, &usound_get_updating_info_res, BEST,
                 usound_get_updating_info);

    MallocString(&usound_upgrade_req.package_md5, REQ, STRING_MAX);
    MallocString(&usound_upgrade_req.total_md5, REQ, STRING_MAX);
    usound_upgrade_req.package_data.data     = file_temp_buff;
    usound_upgrade_req.package_data.size     = 0;
    usound_upgrade_req.package_data.capacity = sizeof(file_temp_buff);

    fal_ota_check_update("main_mcu_temp", &main_firmware_info, &main_updating_info, SOFTWARE_VERSION);
}

void fal_ota_check_update(char *part_name, firmware_info_t *firmwareinfo, updating_info_t *updatinginfo, char *version) {
    fdb_kv_get_blob(&kvdb, part_name, fdb_blob_make(&blob, firmwareinfo, sizeof(firmware_info_t)));
    fdb_kv_get_blob(&kvdb, part_name, fdb_blob_make(&blob, updatinginfo, sizeof(updating_info_t)));

    /*版本不一致视为更新成功*/
    if (strcmp(version, firmwareinfo->version) != 0) {
        /*删除续传信息*/
        memset(updatinginfo, 0, sizeof(updating_info_t));
        fdb_kv_set_blob(&kvdb, part_name, fdb_blob_make(&blob, updatinginfo, sizeof(updating_info_t)));
        /*OTA完成后，修改软件更新时间*/
        pal_uros_msg_set_timestamp(&firmwareinfo->stamp);
        strcpy(firmwareinfo->version, version);
        fdb_kv_set_blob(&kvdb, part_name, fdb_blob_make(&blob, firmwareinfo, sizeof(firmware_info_t)));
        /*发布升级结果*/
        log_i("version update: %s", firmwareinfo->version);
    }
}

static int write_file(char *part_name, uint32_t addr, uint8_t *buffer, uint32_t count) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find(part_name);

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", part_name);
        return -1;
    }

    return fal_partition_write(partition, addr, buffer, count);
}

static void read_file(char *part_name, uint32_t addr, uint8_t *buffer, uint32_t count) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find(part_name);

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", part_name);
        return;
    }

    fal_partition_read(partition, addr, buffer, count);
}

void erase_file(char *part_name) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find(part_name);

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", part_name);
        return;
    }

    if (fal_partition_erase_all(partition) < 0) {
        log_w("Partition (%s) erase failed!\n", partition->name);
        return;
    }

    log_i("Erase (%s) partition finish!\n", partition->name);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), erase_file, erase_file, erase_file);

static int microros_file_receive(char *part_name, updating_info_t *updatinginfo, chassis_interfaces__srv__Upgrade_Request *req_in) {
    if (req_in->package_data.size != req_in->package_size) {
        log_w("error message, package_data.size:%d, package_size:%ld", req_in->package_data.size, req_in->package_size);
        return 1;
    }

    if ((req_in->package_size >= req_in->total_size) || (req_in->package_size > OTA_PACKSIZE)) {
        log_w("error message, package_size:%ld, total_size:%ld, ota_packsize:%d", req_in->package_size, req_in->total_size, OTA_PACKSIZE);
        return 1;
    }

    /*计算接受文件md5*/
    char    md5_cal_temp[33] = {0};
    char    string_temp[3]   = {0};
    uint8_t decrypt[16]      = {0};
    MD5_CTX md5;

    MD5Init(&md5);
    MD5Update(&md5, req_in->package_data.data, req_in->package_size);
    MD5Final(&md5, decrypt);

    for (uint8_t i = 0; i < 16; i++) {
        sprintf(string_temp, "%02x", decrypt[i]);
        strcat(md5_cal_temp, string_temp);
    }

    if (memcmp(md5_cal_temp, req_in->package_md5.data, 32)) {
        log_w("md5 cal :%s", md5_cal_temp);
        log_w("md5 receive :%s", req_in->package_md5.data);
        return 1;
    } else {
        /*认为第一次接受文件*/
        if (req_in->package_offset == 0) {
            /*存储待升级文件总md5*/
            memset(updatinginfo->total_md5, 0, sizeof(updatinginfo->total_md5));
            memcpy(updatinginfo->total_md5, req_in->total_md5.data, req_in->total_md5.size);
            /*擦除备份区*/
            log_i("ota erase file temp");
            erase_file(part_name);
            updatinginfo->buffered_size = 0;
        }

        /*更新已写入文件大小*/
        if (write_file(part_name, req_in->package_offset, (uint8_t *) req_in->package_data.data, req_in->package_size) < 0) {
            log_w("flash write 0x%8lx error", req_in->package_offset);
            return 1;
        } else {
            log_i("flash write 0x%8lx ok, package_size: %ld", req_in->package_offset, req_in->package_size);
            log_i("ota receive %ld percent", (updatinginfo->buffered_size * 100 / req_in->total_size));

            updatinginfo->buffered_size += req_in->package_size;
        }

        fdb_kv_set_blob(&kvdb, part_name, fdb_blob_make(&blob, &updatinginfo, sizeof(updatinginfo)));

        /*升级文件已完全写入*/
        if (updatinginfo->buffered_size == req_in->total_size) {
            log_i("ota receive all %ld Byte ok", req_in->total_size);

            memset(&md5, 0, sizeof(MD5_CTX));
            MD5Init(&md5);
            uint32_t i = 0;

            for (i = 0; i < req_in->total_size / OTA_PACKSIZE; i++) {
                read_file(part_name, i * OTA_PACKSIZE, file_temp_buff, OTA_PACKSIZE);
                MD5Update(&md5, file_temp_buff, OTA_PACKSIZE);
            }

            read_file(part_name, i * OTA_PACKSIZE, file_temp_buff, req_in->total_size - i * OTA_PACKSIZE);
            MD5Update(&md5, file_temp_buff, req_in->total_size - i * OTA_PACKSIZE);
            MD5Final(&md5, decrypt);
            memset(md5_cal_temp, 0, sizeof(md5_cal_temp));

            for (uint8_t i = 0; i < 16; i++) {
                sprintf(string_temp, "%02x", decrypt[i]);
                strcat(md5_cal_temp, string_temp);
            }

            if (memcmp(md5_cal_temp, req_in->total_md5.data, 32)) {
                log_w("total md5 cal : %s", md5_cal_temp);
                log_w("total md5 receive : %s", req_in->total_md5.data);
                return 1;
            } else {
                log_i("md5 ok , start ota");
                updatinginfo->total_size  = req_in->total_size;
                updatinginfo->is_finished = 1;
                updatinginfo->finish_time = osKernelGetTickCount();
            }
        }
    }

    return 0;
}

static void microros_file_getinfo(char *part_name, updating_info_t *updatinginfo, chassis_interfaces__srv__GetUpdatingInfo_Response *res_in) {
    res_in->total_md5.data = updatinginfo->total_md5;
    res_in->total_md5.size = strlen(res_in->total_md5.data);

    /*计算文件md5*/
    char     string_temp[3] = {0};
    uint8_t  decrypt[16]    = {0};
    MD5_CTX  md5;
    uint32_t i = 0;

    MD5Init(&md5);

    for (i = 0; i < updatinginfo->buffered_size / OTA_PACKSIZE; i++) {
        read_file(part_name, i * OTA_PACKSIZE, file_temp_buff, OTA_PACKSIZE);
        MD5Update(&md5, file_temp_buff, OTA_PACKSIZE);
    }

    read_file(part_name, i * OTA_PACKSIZE, file_temp_buff, updatinginfo->buffered_size - i * OTA_PACKSIZE);
    MD5Update(&md5, file_temp_buff, updatinginfo->buffered_size - i * OTA_PACKSIZE);
    MD5Final(&md5, decrypt);

    memset(updatinginfo->buffered_md5, 0, sizeof(updatinginfo->buffered_md5));

    for (uint8_t i = 0; i < 16; i++) {
        sprintf(string_temp, "%02x", decrypt[i]);
        strcat(updatinginfo->buffered_md5, string_temp);
    }

    res_in->buffered_md5.data = updatinginfo->buffered_md5;
    res_in->buffered_md5.size = strlen(res_in->buffered_md5.data);

    res_in->buffered_size = updatinginfo->buffered_size;

    log_i("total_md5:%s", res_in->total_md5.data);
    log_i("buffered_md5:%s", res_in->buffered_md5.data);
    log_i("buffered_size:%ld", res_in->buffered_size);
}

void get_ota_version(void) {
    strcpy(main_firmware_info.version, SOFTWARE_VERSION);
    log_i("main_firmware_info.version :%s", main_firmware_info.version);
    log_i("motor_firmware_info.version :%s", motor_firmware_info.version);
    strcpy(usound_firmware_info.version, usound_version);
    log_i("usound_firmware_info.version :%s", usound_firmware_info.version);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), get_ota_version, get_ota_version, get_ota_version);

void fal_ota_discovered(const void *msgin) {
    log_i("OTA discoverd");
    get_ota_version();
    /*主MCU升级*/
    firmware_info.header.stamp                     = main_firmware_info.stamp;
    firmware_info.priority                         = 0;
    firmware_info.package_size                     = OTA_PACKSIZE;
    firmware_info.app_key.data                     = MAIN_APP_KEY;
    firmware_info.app_key.size                     = strlen(firmware_info.app_key.data);
    firmware_info.version.data                     = main_firmware_info.version;
    firmware_info.version.size                     = strlen(firmware_info.version.data);
    firmware_info.platform.data                    = "android_rom";
    firmware_info.platform.size                    = strlen(firmware_info.platform.data);
    firmware_info.upgrade_service_name.data        = "/ota_" MAIN_APP_KEY "/upgrade";
    firmware_info.upgrade_service_name.size        = strlen(firmware_info.upgrade_service_name.data);
    firmware_info.updating_info_service_name.data  = "/ota_" MAIN_APP_KEY "/get_updating_info";
    firmware_info.updating_info_service_name.size  = strlen(firmware_info.updating_info_service_name.data);
    firmware_info.upgrade_progress_topic_name.data = "/ota_" MAIN_APP_KEY "/upgrade_progress";
    firmware_info.upgrade_progress_topic_name.size = strlen(firmware_info.upgrade_progress_topic_name.data);
    /*接受到discover请求后，发送固件信息topic*/
    message_publish(&g_firmware_info);

    /*驱动MCU升级*/
    firmware_info.header.stamp                     = motor_firmware_info.stamp;
    firmware_info.priority                         = 3;
    firmware_info.package_size                     = OTA_PACKSIZE;
    firmware_info.app_key.data                     = MOTOR_APP_KEY;
    firmware_info.app_key.size                     = strlen(firmware_info.app_key.data);
    firmware_info.version.data                     = motor_firmware_info.version;
    firmware_info.version.size                     = strlen(firmware_info.version.data);
    firmware_info.platform.data                    = "android_rom";
    firmware_info.platform.size                    = strlen(firmware_info.platform.data);
    firmware_info.upgrade_service_name.data        = "/ota_" MOTOR_APP_KEY "/upgrade";
    firmware_info.upgrade_service_name.size        = strlen(firmware_info.upgrade_service_name.data);
    firmware_info.updating_info_service_name.data  = "/ota_" MOTOR_APP_KEY "/get_updating_info";
    firmware_info.updating_info_service_name.size  = strlen(firmware_info.updating_info_service_name.data);
    firmware_info.upgrade_progress_topic_name.data = "/ota_" MOTOR_APP_KEY "/upgrade_progress";
    firmware_info.upgrade_progress_topic_name.size = strlen(firmware_info.upgrade_progress_topic_name.data);
    /*接受到discover请求后，发送固件信息topic*/
    message_publish(&g_firmware_info);

    /*超声MCU升级*/
    firmware_info.header.stamp                     = usound_firmware_info.stamp;
    firmware_info.priority                         = 3;
    firmware_info.package_size                     = OTA_PACKSIZE;
    firmware_info.app_key.data                     = USOUND_APP_KEY;
    firmware_info.app_key.size                     = strlen(firmware_info.app_key.data);
    firmware_info.version.data                     = usound_firmware_info.version;
    firmware_info.version.size                     = strlen(firmware_info.version.data);
    firmware_info.platform.data                    = "android_rom";
    firmware_info.platform.size                    = strlen(firmware_info.platform.data);
    firmware_info.upgrade_service_name.data        = "/ota_" USOUND_APP_KEY "/upgrade";
    firmware_info.upgrade_service_name.size        = strlen(firmware_info.upgrade_service_name.data);
    firmware_info.updating_info_service_name.data  = "/ota_" USOUND_APP_KEY "/get_updating_info";
    firmware_info.updating_info_service_name.size  = strlen(firmware_info.updating_info_service_name.data);
    firmware_info.upgrade_progress_topic_name.data = "/ota_" USOUND_APP_KEY "/upgrade_progress";
    firmware_info.upgrade_progress_topic_name.size = strlen(firmware_info.upgrade_progress_topic_name.data);
    /*接受到discover请求后，发送固件信息topic*/
    message_publish(&g_firmware_info);
}
/*主MCU升级*/
void main_upgrade(const void *req, void *res) {
    chassis_interfaces__srv__Upgrade_Request * req_in = (chassis_interfaces__srv__Upgrade_Request *) req;
    chassis_interfaces__srv__Upgrade_Response *res_in = (chassis_interfaces__srv__Upgrade_Response *) res;

    if (microros_file_receive("main_mcu_temp", &main_updating_info, req_in) == 0) {
        res_in->success = 1;
    } else {
        res_in->success = 0;
    }
}
void main_get_updating_info(const void *req, void *res) {
    // chassis_interfaces__srv__GetUpdatingInfo_Request * req_in =
    // (chassis_interfaces__srv__GetUpdatingInfo_Request *) req;
    chassis_interfaces__srv__GetUpdatingInfo_Response *res_in = (chassis_interfaces__srv__GetUpdatingInfo_Response *) res;

    microros_file_getinfo("main_mcu_temp", &main_updating_info, res_in);
}

/*驱动器MCU升级*/
void motor_upgrade(const void *req, void *res) {
    chassis_interfaces__srv__Upgrade_Request * req_in = (chassis_interfaces__srv__Upgrade_Request *) req;
    chassis_interfaces__srv__Upgrade_Response *res_in = (chassis_interfaces__srv__Upgrade_Response *) res;

    if (microros_file_receive("motordriver_temp", &motor_updating_info, req_in) == 0) {
        res_in->success = 1;
    } else {
        res_in->success = 0;
    }
}
void motor_get_updating_info(const void *req, void *res) {
    // chassis_interfaces__srv__GetUpdatingInfo_Request * req_in =
    // (chassis_interfaces__srv__GetUpdatingInfo_Request *) req;
    chassis_interfaces__srv__GetUpdatingInfo_Response *res_in = (chassis_interfaces__srv__GetUpdatingInfo_Response *) res;

    microros_file_getinfo("motordriver_temp", &motor_updating_info, res_in);
}

/*超声MCU升级*/
void usound_upgrade(const void *req, void *res) {
    chassis_interfaces__srv__Upgrade_Request * req_in = (chassis_interfaces__srv__Upgrade_Request *) req;
    chassis_interfaces__srv__Upgrade_Response *res_in = (chassis_interfaces__srv__Upgrade_Response *) res;

    if (microros_file_receive("usound_temp", &usound_updating_info, req_in) == 0) {
        res_in->success = 1;
    } else {
        res_in->success = 0;
    }
}
void usound_get_updating_info(const void *req, void *res) {
    // chassis_interfaces__srv__GetUpdatingInfo_Request * req_in =
    // (chassis_interfaces__srv__GetUpdatingInfo_Request *) req;
    chassis_interfaces__srv__GetUpdatingInfo_Response *res_in = (chassis_interfaces__srv__GetUpdatingInfo_Response *) res;

    microros_file_getinfo("usound_temp", &usound_updating_info, res_in);
}