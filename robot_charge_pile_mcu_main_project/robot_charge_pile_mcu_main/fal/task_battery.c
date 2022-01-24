/******************************************************************************
 * @Function: task_battery.c
 * @Description: 电池任务入口文件
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/

#include "task_battery.h"
#include "task_led.h"
#include "task_charge.h"
#include "fal_charge_wire.h"
#include "fal_charge_auto.h"
#include "drv_can.h"
#include "drv_gpio.h"
#include "crc16.h"
#include "log.h"
#include "common_def.h"
#include "cmsis_os.h"
#include <string.h>
#include "stm32f1xx_hal_can.h"

#define BMS_INFO_END_BIT 1

osEventFlagsId_t sys_event_battery;
battery_var_stu_t battery_var;

static uint8_t rx_len = 0;

/******************************************************************************
 * @Function: battery_init
 * @Description: 电池初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void battery_init(void) {
    sys_event_battery = osEventFlagsNew(NULL);
    memset(&battery_var, 0, sizeof(battery_var_stu_t));
}

/******************************************************************************
 * @Function: data_big2little
 * @Description: 大端转小端函数
 * @Input: data：起始地址
 *         size：数据大小
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *data
 * @param {uint8_t} size
 *******************************************************************************/
void data_big2little(uint8_t *data, uint8_t size) {
    uint8_t temp[2];
    uint8_t i;

    for (i = 0; i < size / 2; i++) {
        temp[0] = data[i];
        temp[1] = data[size - i - 1];

        data[i] = temp[1];
        data[size - i - 1] = temp[0];
    }
}

/******************************************************************************
 * @Function: battery_info_big2little
 * @Description: 电池信息大端转小端
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {battery_info_stu_t} *info
 *******************************************************************************/
void battery_info_big2little(battery_info_stu_t *info) {
    data_big2little((uint8_t *) &info->current, sizeof(info->current));
    data_big2little((uint8_t *) &info->cap_full_in, sizeof(info->cap_full_in));
    data_big2little((uint8_t *) &info->cap_full_out,
                    sizeof(info->cap_full_out));
    data_big2little((uint8_t *) &info->cap_remain, sizeof(info->cap_remain));
    data_big2little((uint8_t *) &info->voltage, sizeof(info->voltage));
    data_big2little((uint8_t *) &info->rsos, sizeof(info->rsos));
    data_big2little((uint8_t *) &info->cyc_time, sizeof(info->cyc_time));
    data_big2little((uint8_t *) &info->sta_protect, sizeof(info->sta_protect));
    data_big2little((uint8_t *) &info->sta_warming, sizeof(info->sta_warming));
    data_big2little((uint8_t *) &info->sta_bms, sizeof(info->sta_bms));
    data_big2little((uint8_t *) &info->sta_bla, sizeof(info->sta_bla));
    data_big2little((uint8_t *) &info->temp_cell1, sizeof(info->temp_cell1));
    data_big2little((uint8_t *) &info->temp_cell2, sizeof(info->temp_cell2));
    data_big2little((uint8_t *) &info->temp_cell3, sizeof(info->temp_cell3));
    data_big2little((uint8_t *) &info->temp_cell4, sizeof(info->temp_cell4));
    data_big2little((uint8_t *) &info->temp_env, sizeof(info->temp_env));
    data_big2little((uint8_t *) &info->temp_power_board1,
                    sizeof(info->temp_power_board1));
    data_big2little((uint8_t *) &info->temp_power_board2,
                    sizeof(info->temp_power_board2));
    data_big2little((uint8_t *) &info->vol_cell1, sizeof(info->vol_cell1));
    data_big2little((uint8_t *) &info->vol_cell2, sizeof(info->vol_cell2));
    data_big2little((uint8_t *) &info->vol_cell3, sizeof(info->vol_cell3));
    data_big2little((uint8_t *) &info->vol_cell4, sizeof(info->vol_cell4));
    data_big2little((uint8_t *) &info->vol_cell5, sizeof(info->vol_cell5));
    data_big2little((uint8_t *) &info->vol_cell6, sizeof(info->vol_cell6));
    data_big2little((uint8_t *) &info->vol_cell7, sizeof(info->vol_cell7));
    data_big2little((uint8_t *) &info->vol_cell8, sizeof(info->vol_cell8));
    data_big2little((uint8_t *) &info->vol_cell9, sizeof(info->vol_cell9));
    data_big2little((uint8_t *) &info->vol_cell10, sizeof(info->vol_cell10));
    data_big2little((uint8_t *) &info->vol_cell11, sizeof(info->vol_cell11));
    data_big2little((uint8_t *) &info->vol_cell12, sizeof(info->vol_cell12));
    data_big2little((uint8_t *) &info->vol_cell13, sizeof(info->vol_cell13));
    data_big2little((uint8_t *) &info->vol_cell14, sizeof(info->vol_cell14));
}

/******************************************************************************
 * @Function: bms_info_update
 * @Description: 更新BMS数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bms_info_update(void) {
    can_msg_stu_t send_msg;
    uint8_t tx_msg[8];

    send_msg.id = BATTERY_BMS_REQUEST_CAN_ID;
    send_msg.type = CAN_ID_STD;
    send_msg.data_len = 8;
    memset(tx_msg, 0, sizeof(tx_msg));
    tx_msg[0] = BATTERY_BMS_PACK_INDEX_START;
    tx_msg[1] = 6;
    tx_msg[2] = BATTERY_DEVICE_ADDR;
    tx_msg[3] = BATTERY_READ_FUNC_CODE;
    tx_msg[4] = 0;
    tx_msg[5] = BATTERY_INFO_START_ADDR;
    tx_msg[6] = 32;
    uint16_t crc = calculate_crc16(&tx_msg[1], 6);
    tx_msg[7] = crc & 0xFF;
    send_msg.data = tx_msg;
    drv_can_send_msg(send_msg);

    memset(tx_msg, 0, sizeof(tx_msg));
    tx_msg[0] = BATTERY_BMS_PACK_INDEX_END;
    tx_msg[1] = (crc >> 8) & 0xFF;
    drv_can_send_msg(send_msg);
}

void battery_info_handle(void) {
    battery_bms_frame_stu_t *bms_frame_ptr = NULL;

    uint32_t sub_evt = osEventFlagsWait(sys_event_battery, BMS_INFO_END_BIT,
                                        osFlagsWaitAny, 100);

    if ((sub_evt & osFlagsError) == 0) {
        uint16_t crc = calculate_crc16(battery_var.bms_data, rx_len - 2);
        if ((battery_var.bms_data[rx_len - 2] == (crc & 0xFF)) &&
            (battery_var.bms_data[rx_len - 1] == (crc >> 8))) {
            bms_frame_ptr = (battery_bms_frame_stu_t *) battery_var.bms_data;
            battery_var.battery_info = bms_frame_ptr->bt_info;
            battery_info_big2little(&battery_var.battery_info);

            rx_len = 0;

            LOG_DEBUG_ARRAY((uint8_t *) &battery_var.battery_info,
                            sizeof(battery_info_stu_t));

            //按键、CAN通信未触发有线充电，电极未压下
            if ((!SYS_GET_BIT(wire_charge_var.flag,
                              CHARGE_WIRE_KEY_TRIGGER_BIT)) &&
                (!SYS_GET_BIT(wire_charge_var.flag,
                              CHARGE_WIRE_COM_TRIGGER_BIT)) &&
                (!SYS_GET_BIT(auto_charge_var.flag,
                              CHARGE_AUTO_ELECTRODE_DOWN_BIT)) &&
                (!SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT))) {
                drv_gpio_wire_charge_set(ENABLE);
                // SYS_CLR_BIT(auto_charge_var.flag,
                // CHARGE_AUTO_SEND_CMD_EN_BIT);
                SYS_SET_BIT(wire_charge_var.flag, CHARGE_WIRE_COM_TRIGGER_BIT);
                LOG_DEBUG("Start wire charge with CAN com!");
            }

            //清除超时计数
            battery_var.battery_info_timeout_cnt = 0;
            //开启超时
            SYS_SET_BIT(battery_var.flag, BATTERY_INFO_REQUEST_UPDATE_BIT);
            LOG_DEBUG("battery ok");
        } else {
            LOG_ERROR("CAN data crc error!");
        }
    }
}

/******************************************************************************
 * @Function: get_battery_charge_stutus_bit
 * @Description: 获取电池充电状态标志位函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t get_battery_charge_stutus_bit(void) {
    return battery_var.battery_info.sta_bms.charge_sta;
}

/******************************************************************************
 * @Function: get_battery_percentage
 * @Description: 获取电池电量百分比函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t get_battery_percentage(void) {
    return (uint8_t)(((float) battery_var.battery_info.cap_remain /
                      (float) battery_var.battery_info.cap_full_in) *
                         100 +
                     0.5);
}

/******************************************************************************
 * @Function: battery_server
 * @Description: 充电电池服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void battery_server(void) {
    static uint32_t bms_info_req_ts;
    if (osKernelGetTickCount() > bms_info_req_ts + 1000) {
        bms_info_req_ts = osKernelGetTickCount();
        bms_info_update();
    }

    battery_info_handle();
}

/******************************************************************************
 * @Function: task_battery_init
 * @Description: 电池任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_battery_init(void) {
    battery_init();

    return 0;
}

/******************************************************************************
 * @Function: task_battery_deInit
 * @Description: 电池任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_battery_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_battery_run
 * @Description: 电池任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_battery_run(void *pvParameters) {
    task_battery_init();
    osDelay(1000);

    while (1) { battery_server(); }
}

void get_battery_info_frame_parse(CAN_RxHeaderTypeDef RxHeader,
                                  uint8_t *rx_msg) {
    static uint8_t rx_frame_index = 0;

    if (rx_msg[0] == rx_frame_index) {
        memcpy(battery_var.bms_data + rx_len, &rx_msg[1], RxHeader.DLC - 1);
        rx_frame_index++;
        rx_len += RxHeader.DLC - 1;
    } else {
        rx_frame_index = 0;
        rx_len = 0;
        memset(battery_var.bms_data, 0, sizeof(BATTERY_INFO_MAX_LEN));
    }

    if ((rx_msg[0] == 0x0A) && (rx_len == 77))  //接受一帧数据完成
    {
        rx_frame_index = 0;
        osEventFlagsSet(sys_event_battery, BMS_INFO_END_BIT);
    }
}

/******************************************************************************
 * @Function: HAL_CAN_RxFifo0MsgPendingCallback
 * @Description: CAN接收中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {CAN_HandleTypeDef} *hcan
 *******************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t rx_msg[8] = {0};
    HAL_StatusTypeDef status;

    status = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rx_msg);
    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return;
    }
    if (RxHeader.StdId == BATTERY_BMS_RESPOND_CAN_ID) {
        get_battery_info_frame_parse(RxHeader, rx_msg);
    } else if (RxHeader.StdId == BATTERY_BMS_REQUEST_CAN_ID) {
    }
}
