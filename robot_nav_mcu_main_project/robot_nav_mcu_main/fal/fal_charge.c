/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-12
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_charge.h"
#include "cmsis_os.h"
#include "sys_exc.h"
#include "sys_paras.h"
#include "sys_pubsub.h"
#include "bsp_simuart_tx.h"
#include "bsp_simuart_rx.h"
#include "tim.h"
#include "gpio.h"
#include "shell.h"
#include "stdio.h"
#include "crc16.h"
#include "pal_uros.h"
#include "std_msgs/msg/empty.h"
#include "std_msgs/msg/bool.h"
#include "std_msgs/msg/float32.h"
#include "std_msgs/msg/u_int8_multi_array.h"
#include "chassis_interfaces/srv/pile_request.h"
#include "std_srvs/srv/set_bool.h"
#include "shell.h"

#define LOG_TAG "fal_charge"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_CHARGE 充电管理模块 - CHARGE
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define IR_38K_RESEND_COUNT 4
/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/
SimUart_Tx_TypeDef simuart1;
SimUart_Rx_TypeDef simuart2;
SimUart_Rx_TypeDef simuart3;

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static uint8_t pbuff[255]      = {0};
static uint8_t pbuff_next[100] = {0};

static uint8_t auto_charge_data_buff[2]     = {0};
static uint8_t auto_charge_ir_txbuff[10]    = {0};
static uint8_t auto_charge_ir_rxbuff[2][10] = {0};

static uint8_t ir_rand;
// static uint8_t ir_update;
static uint8_t  g_ir_ptr[2]     = {0};
static uint8_t  ir_handshake_ok = 0;
static uint8_t  ir_check_ok[2]  = {0};
static uint32_t rx_reset_ts     = 0;
static bool     is_charge       = false;

static publisher                      g_uros_ir_message;
static std_msgs__msg__UInt8MultiArray uros_ir_message;
static publisher                      g_uros_charge_feedback;
static std_msgs__msg__UInt8           uros_charge_feedback;
bool                                  uros_charge_feedback_send_flag = false;

static publisher           g_uros_cable_state;
static std_msgs__msg__Bool uros_cable_state;
bool                       uros_cable_state_send_flag = false;

static subscrption         g_cable_state_update;
static std_msgs__msg__Bool cable_state_update;

static service                                g_pile_request;
chassis_interfaces__srv__PileRequest_Request  pile_request_req = {0};
chassis_interfaces__srv__PileRequest_Response pile_request_res = {0};

static service                  g_charge_control;
std_srvs__srv__SetBool_Request  charge_control_req = {0};
std_srvs__srv__SetBool_Response charge_control_res = {0};

static uint32_t      auto_charge_running_delay = 0;
static CHARGE_CTRL_T charge_ctrl;

static publisher                      g_bluetooth_addr;
static std_msgs__msg__UInt8MultiArray bluetooth_addr;
static uint8_t                        bluetooth_addr_buff[10] = {1, 2, 3, 4, 5, 6, 1, 2, 3, 4};
static uint8_t                        bluetooth_pub_flag      = 0;

static subscrption         g_bluetooth_addr_update;
static std_msgs__msg__Bool bluetooth_addr_update;

static uint8_t auto_charge_packing_req = 0;

uint8_t pub_blue_mac(void) {
    bluetooth_pub_flag = 1;
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pub_blue_mac, pub_blue_mac, pub_blue_mac);

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void     ir1_rx_callback(uint8_t data);
static void     ir2_rx_callback(uint8_t data);
static uint32_t get_timecount(void);

static void ir_send_message(uint8_t head, uint16_t pile_id);
static void ir_send_selfcheck(uint8_t head);
void        uint8buffprint(char *mess, uint8_t *buff, uint8_t len);

static uint8_t fal_charge_pull_in(ir_38k_send_cmd_enum_t cmd);

static void pile_request_service_callback(const void *req, void *res);
static void charge_control_service_callback(const void *req, void *res);
static void cable_state_update_subscription_callback(const void *msgin);
static void bluetooth_addr_update_subscription_callback(const void *msgin);
uint8_t     fal_charge_set_packing_req(uint8_t is_req);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_charge_init
 * Description:    初始化FAL层
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_charge_init(void) {
    publisher_init(&g_uros_ir_message, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8MultiArray), "/auto_charge/ir_message", &uros_ir_message,
                   BEST, OVERWRITE, sizeof(std_msgs__msg__UInt8MultiArray));

    publisher_init(&g_uros_charge_feedback, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/auto_charge/charge_feedback",
                   &uros_charge_feedback, DEFAULT, SEND, sizeof(std_msgs__msg__UInt8));

    publisher_init(&g_uros_cable_state, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/auto_charge/cable_state", &uros_cable_state,
                   DEFAULT, SEND, sizeof(std_msgs__msg__Bool));

    subscrption_init(&g_bluetooth_addr_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/auto_charge/bluetooth/addr/update",
                     &bluetooth_addr_update, DEFAULT, bluetooth_addr_update_subscription_callback);

    subscrption_init(&g_cable_state_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/auto_charge/cable_state/update",
                     &cable_state_update, DEFAULT, cable_state_update_subscription_callback);

    publisher_init(&g_bluetooth_addr, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8MultiArray), "/auto_charge/bluetooth/addr",
                   &bluetooth_addr, BEST, OVERWRITE, sizeof(std_msgs__msg__UInt8MultiArray));

    service_init(&g_pile_request, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, PileRequest), "/auto_charge/pile_request",
                 &pile_request_req, &pile_request_res, BEST, pile_request_service_callback);

    service_init(&g_charge_control, ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool), "/auto_charge/charge_control", &charge_control_req,
                 &charge_control_res, BEST, charge_control_service_callback);

    bluetooth_addr.data.data = bluetooth_addr_buff;
    bluetooth_addr.data.size = sizeof(bluetooth_addr_buff);

    /*添加模块处理函数*/
    simuart1.baudrate       = 1200;
    simuart1.gpio_port      = MX_TIM1_CH1_IR_TX1_GPIO_Port;
    simuart1.gpio_pin       = MX_TIM1_CH1_IR_TX1_Pin;
    simuart1.pwm_mode       = 1;
    simuart1.tx_pwm_tim     = htim1;
    simuart1.tx_pwm_channel = TIM_CHANNEL_1;
    simuart1.tim            = htim4;
    simuart1.frq_div_clock  = 1000000;

    bsp_simuart_tx_init(&simuart1);

    simuart2.baudrate         = 1200;
    simuart2.gpio_port        = MX_IR_SIM_UART_RX2_GPIO_Port;
    simuart2.gpio_pin         = MX_IR_SIM_UART_RX2_Pin;
    simuart2.tim              = htim2;
    simuart2.frq_div_clock    = 1000000;
    simuart2.receive_callback = ir1_rx_callback;
    simuart2.get_timecount    = get_timecount;

    bsp_simuart_rx_init(&simuart2);

    simuart3.baudrate         = 1200;
    simuart3.gpio_port        = MX_IR_SIM_UART_RX1_GPIO_Port;
    simuart3.gpio_pin         = MX_IR_SIM_UART_RX1_Pin;
    simuart3.tim              = htim3;
    simuart3.frq_div_clock    = 1000000;
    simuart3.receive_callback = ir2_rx_callback;
    simuart3.get_timecount    = get_timecount;

    bsp_simuart_rx_init(&simuart3);

    if (HAL_GPIO_ReadPin(MX_CHARGE_DET_GPIO_Port, MX_CHARGE_DET_Pin) == GPIO_PIN_SET) {
        log_i("CHARGE_DET OFF, stop charge");
        fal_charge_relay_off();
    } else {
        log_i("CHARGE_DET ON, start charge");
        fal_charge_relay_on();
    }

    ir_rand = 0xA0;
    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_charge_deInit
 * Description:    释放 fal_charge 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_charge_deInit(void) {
    return 0;
}

void pile_request_service_callback(const void *req, void *res) {
    chassis_interfaces__srv__PileRequest_Request * req_in = (chassis_interfaces__srv__PileRequest_Request *) req;
    chassis_interfaces__srv__PileRequest_Response *res_in = (chassis_interfaces__srv__PileRequest_Response *) res;

    log_i("Service request pile_id: %d\n", (int) req_in->pile_id);

    CHARGE_CTRL_T ctrl;
    ctrl.pile_id = req_in->pile_id;
    pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);

    res_in->success = 1;
}

void charge_control_service_callback(const void *req, void *res) {
    std_srvs__srv__SetBool_Request * req_in = (std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response *res_in = (std_srvs__srv__SetBool_Response *) res;

    log_i("charge_control: %d\n", req_in->data);

    if (req_in->data) {
        fal_charge_relay_on();
    } else {
        fal_charge_relay_off();
    }

    res_in->success = 1;
}

void set_charge_feedback(CHARGE_FEEDBACK_E feedback) {
    uros_charge_feedback.data      = feedback;
    uros_charge_feedback_send_flag = true;
    log_i("set feedback: %d", uros_charge_feedback.data);
}

/*****************************************************************/
/**
 * Function:       get_charge_cable_status
 * Description:    获取线充电缆插入状态
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @retur
 *   false 线充电缆已拔出
 *   true  线充电缆已插入
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
CHARGE_CABLE_STATE get_charge_cable_status(void) {
    static CHARGE_CABLE_STATE cable_state = CHARGE_CABLE_STATE_CANCEL;
    static uint8_t            io_status   = CABLE_CHARGE_ALL_BIT_HIGH;

    io_status = (io_status << 1) | HAL_GPIO_ReadPin(MX_CHARGE_DET_MANUAL_GPIO_Port, MX_CHARGE_DET_MANUAL_Pin);
    if (io_status == CABLE_CHARGE_ALL_BIT_LOW && cable_state != CHARGE_CABLE_STATE_INSERT) {
        cable_state = CHARGE_CABLE_STATE_INSERT;
    } else if (io_status == CABLE_CHARGE_ALL_BIT_HIGH && cable_state != CHARGE_CABLE_STATE_CANCEL) {
        cable_state = CHARGE_CABLE_STATE_CANCEL;
    }

    return cable_state;
}

void task_fal_charge_run(void *argument) {
    uint32_t  sub_evt;
    PMU_STA_T pmu_sta;

    uint32_t charge_loc_update_ts = 0;
    uint32_t print_ts             = 0;
    uint32_t ir_check_ts          = 0;
    // uint32_t mac_send_ts          = 0;
    uint32_t ir_handshake_ts        = 0;
    uint32_t feed_back_send_ts      = 0;
    uint32_t auto_charge_packing_ts = 0;

    uint8_t ir_send_start  = IR_38K_RESEND_COUNT;
    uint8_t ir_check_start = 0;

    fal_charge_init();

    for (;;) {
        /// < 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_charge, SYS_EVT_AUTO_CHARGE | SYS_EVT_PMU_STA, osFlagsWaitAny, 50);

        if ((sub_evt & osFlagsError) == 0) {
            ///< 读取主题数据

            ///< 充电控制主题
            if (sub_evt & SYS_EVT_AUTO_CHARGE) {
                xQueuePeek(topic_charge, &charge_ctrl, 100);

                /*收到对桩请求命令1s内开启红外位置更新，1s内电极压下，视为对桩成功*/
                auto_charge_running_delay = osKernelGetTickCount() + 120000;
                log_i("38K tx, pile_id = 0x%2x, ir_start:%d, ir_rand:0x%2x", charge_ctrl.pile_id, ir_send_start, ir_rand);
                auto_charge_data_buff[0] = 0;
                auto_charge_data_buff[1] = 0;

                if (++ir_send_start >= IR_38K_RESEND_COUNT) {
                    /*200ms内判断是否收到握手响应*/
                    ir_handshake_ts = osKernelGetTickCount() + 200;
                    ir_handshake_ok = 0;
                    ir_send_message(0x55, charge_ctrl.pile_id);
                }
            }

            ///< 开关机状态主题
            if (sub_evt & SYS_EVT_PMU_STA) {
                xQueuePeek(topic_pmu_sta, &pmu_sta, 100);

                ///< 开机红外收发自检
                if (pmu_sta.sta == PMU_STA_STARTING) {
                    ir_check_start = 1;
                }
            }

            ///< 开启充电状态主题
        }
        if (osKernelGetTickCount() >= auto_charge_packing_ts) {
            auto_charge_packing_ts = osKernelGetTickCount() + 1000;
            if (auto_charge_packing_req > 0) {
                auto_charge_packing_req--;
            }
        }

        if ((osKernelGetTickCount() <= ir_handshake_ts) && (ir_handshake_ok == 1)) {
            ir_send_message(0x56, charge_ctrl.pile_id);
            ir_handshake_ok = 0;
        }

        static CHARGE_CABLE_STATE cable_state_last = CHARGE_CABLE_STATE_CANCEL;
        CHARGE_CABLE_STATE        cable_state      = get_charge_cable_status();
        if (cable_state_last != cable_state) {
            cable_state_last           = cable_state;
            uros_cable_state.data      = cable_state_last;
            uros_cable_state_send_flag = true;
        }

        if (cable_state == CHARGE_CABLE_STATE_INSERT) {
            fal_charge_set_packing_req(3);
        }

        if (uros_cable_state_send_flag) {
            uros_cable_state_send_flag = false;
            log_i("pub cable_state: %d", uros_cable_state.data);
            message_publish(&g_uros_cable_state);
        }

        if (uros_charge_feedback_send_flag) {
            uros_charge_feedback_send_flag = false;
            static uint8_t data_last       = 0;
            if (uros_charge_feedback.data != data_last || osKernelGetTickCount() - feed_back_send_ts > 2000) {
                log_i("pub charge_feedback: %02X", uros_charge_feedback.data);
                message_publish(&g_uros_charge_feedback);
                data_last = uros_charge_feedback.data;
            }
            feed_back_send_ts = osKernelGetTickCount();
        }

        /* 发布蓝牙MAC和PIN */
        if (bluetooth_pub_flag) {
            bluetooth_pub_flag = 0;
            // if (osKernelGetTickCount() - mac_send_ts > 2000) {
            message_publish(&g_bluetooth_addr);
            // }
            // mac_send_ts = osKernelGetTickCount();
        }

        if (osKernelGetTickCount() <= auto_charge_running_delay) {
            if (osKernelGetTickCount() >= charge_loc_update_ts) {
                charge_loc_update_ts = osKernelGetTickCount() + sys_paras.auto_charge_loc_update_ms;

                static uint8_t ir_buff[2];
                ir_buff[0]                = auto_charge_data_buff[0];
                ir_buff[1]                = auto_charge_data_buff[1];
                uros_ir_message.data.data = ir_buff;
                uros_ir_message.data.size = sizeof(ir_buff);
                message_publish(&g_uros_ir_message);
                memset(auto_charge_data_buff, 0, sizeof(auto_charge_data_buff));
                log_i("uros_ir_message :0x%2x, 0x%2x", uros_ir_message.data.data[0], uros_ir_message.data.data[1]);

                ir_send_start = 0;
            }
        }

        if (osKernelGetTickCount() >= rx_reset_ts) {
            memset(g_ir_ptr, 0, sizeof(g_ir_ptr));
        }

        extern uint8_t  auto_send_flag;
        extern uint16_t auto_send_time_ms;
        extern uint16_t auto_send_pile_id;

        if ((osKernelGetTickCount() >= print_ts) && auto_send_flag) {
            print_ts       = osKernelGetTickCount() + auto_send_time_ms;
            auto_send_flag = 0;
            CHARGE_CTRL_T ctrl;
            ctrl.pile_id = (CHARGE_CTRL_E) auto_send_pile_id;
            pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);
            log_i("tx_send");
        }

        /*开机红外自检*/
        if ((osKernelGetTickCount() > ir_check_ts) && ir_check_start) {
            ir_check_ts = osKernelGetTickCount() + 1000;
            ir_send_selfcheck(ir_rand);

            if (ir_check_ok[0] && ir_check_ok[1]) {
                ir_check_ts = 0xFFFFFFFF;
                log_i("[OK] Robot selftest: IR");
                sys_exc_clear(EXC50_NVG_IR);
            } else {
                log_e("[ERROR] Robot selftest: IR");
                sys_exc_set(EXC50_NVG_IR);
            }
        }
    }
}

static uint32_t get_timecount(void) {
    return osKernelGetTickCount();
}

void rx_data_parse(const uint8_t rx_index, const uint8_t data) {
    uint8_t *ir_buff = auto_charge_ir_rxbuff[rx_index];
    uint8_t *ir_ptr  = &g_ir_ptr[rx_index];

    rx_reset_ts          = osKernelGetTickCount() + 5;
    ir_buff[(*ir_ptr)++] = data;

    if (ir_buff[0] != ir_rand) {
        *ir_ptr = 0;
    }

    if (ir_buff[1] == 0x5F || ir_buff[1] == 0xF6 || ir_buff[1] == 0x56) {
        //接受到红外信号
        *ir_ptr                         = 0;
        ir_check_ok[rx_index]           = 1;
        auto_charge_data_buff[rx_index] = ir_buff[1];
        ir_buff[1]                      = 0;
    } else if (ir_buff[1] == SEND_BLE_MAC_CMD) {
        if (*ir_ptr >= 9) {
            *ir_ptr           = 0;
            uint8_t check_sum = 0;
            for (int i = 0; i < 8; i++) {
                check_sum += ir_buff[i];
            }
            if (check_sum == ir_buff[8]) {
                log_i("rx%d mac:%02X %02X %02X %02X %02X %02X", rx_index, ir_buff[2], ir_buff[3], ir_buff[4], ir_buff[5], ir_buff[6],
                      ir_buff[7]);

                if (bluetooth_pub_flag != 1) {
                    memcpy(bluetooth_addr_buff, &ir_buff[2], 6);
                    bluetooth_addr_buff[6] = 1;
                    bluetooth_addr_buff[7] = 2;
                    bluetooth_addr_buff[8] = 3;
                    bluetooth_addr_buff[9] = 4;

                    bluetooth_pub_flag = 1;
                }
            }
            ir_buff[1] = 0;
        }
    } else if (ir_buff[1] == SEND_ELECTRODE_DOWN_CMD || ir_buff[1] == SEND_ELECTRODE_UP_CMD) {
        if (*ir_ptr >= 3) {
            *ir_ptr = 0;
            if (ir_buff[1] + ir_buff[2] == 255) {
                //电极压下或弹开
                fal_charge_pull_in(ir_buff[1]);
            }
            ir_buff[1] = 0;
        }
    } else if (ir_buff[1] == SEND_LOC_SEND_EN_ACK_CMD) {
        if (*ir_ptr >= 3) {
            *ir_ptr = 0;
            if (ir_buff[1] + ir_buff[2] == 255) {
                //握手成功
                ir_check_ok[rx_index] = 1;
                ir_handshake_ok       = 1;
            }
            ir_buff[1] = 0;
        }
    } else if (*ir_ptr >= 2) {
        *ir_ptr = 0;
    }

    log_d("rx%d: %2x", rx_index, data);
}

static void ir1_rx_callback(uint8_t data) {
    rx_data_parse(0, data);
}

static void ir2_rx_callback(uint8_t data) {
    rx_data_parse(1, data);
}

static void ir_send_message(uint8_t head, uint16_t pile_id) {
    auto_charge_ir_txbuff[0] = head;
    auto_charge_ir_txbuff[1] = (uint8_t)(pile_id >> 8);
    auto_charge_ir_txbuff[2] = (uint8_t) pile_id;
    auto_charge_ir_txbuff[3] = ir_rand;
    auto_charge_ir_txbuff[4] = (uint8_t) calculate_crc16(auto_charge_ir_txbuff, 4);

    bsp_simuart_tx_send_buff(&simuart1, auto_charge_ir_txbuff, 5);
}

static void ir_send_selfcheck(uint8_t head) {
    auto_charge_ir_txbuff[0] = head;
    auto_charge_ir_txbuff[1] = SEND_LOC_SEND_EN_ACK_CMD;
    auto_charge_ir_txbuff[2] = ~auto_charge_ir_txbuff[1];

    bsp_simuart_tx_send_buff(&simuart1, auto_charge_ir_txbuff, 3);
}

void uint8buffprint(char *mess, uint8_t *buff, uint8_t len) {
    memset((char *) pbuff, 0, sizeof(pbuff));
    strcat((char *) pbuff, mess);

    for (uint8_t i = 0; i < len; i++) {
        sprintf((char *) pbuff_next, "buff[%d]:%2x ", i, buff[i]);
        strcat((char *) pbuff, (char *) pbuff_next);
    }

    log_d("%s", (char *) pbuff);
}

uint8_t fal_charge_pull_in(ir_38k_send_cmd_enum_t cmd) {
    if (cmd == SEND_ELECTRODE_DOWN_CMD) {
        if (osKernelGetTickCount() <= auto_charge_running_delay) {
            // log_i("Auto charge eletrode is pressed.");
            set_charge_feedback(AUTO_CHARGE_ELEC_PRESS);
            fal_charge_set_packing_req(3);
            auto_charge_running_delay = osKernelGetTickCount() + 500;
        } else {
            // log_i("Push charge eletrode is pressed.");
            set_charge_feedback(PUSH_CHARGE_ELEC_PRESS);
        }
    } else if (cmd == SEND_ELECTRODE_UP_CMD) {
        // log_i("charge eletrode up");
        set_charge_feedback(CHARGE_ELEC_UP);
    }

    ir_rand = 0xA0;
    return 0;
}

uint8_t  auto_send_flag    = 0;
uint16_t auto_send_time_ms = 0;
uint16_t auto_send_pile_id = 0;
uint8_t  ir_38k_auto_send(uint16_t time, uint16_t pile_id, uint8_t flag) {
    auto_send_flag    = flag;
    auto_send_time_ms = time;
    auto_send_pile_id = pile_id;
    return 0;
}

void fal_charge_relay_on(void) {
    is_charge = true;
    HAL_GPIO_WritePin(MX_CHARGE_RELAY_CTRL_GPIO_Port, MX_CHARGE_RELAY_CTRL_Pin, GPIO_PIN_RESET);
}

void fal_charge_relay_off(void) {
    is_charge = false;
    HAL_GPIO_WritePin(MX_CHARGE_RELAY_CTRL_GPIO_Port, MX_CHARGE_RELAY_CTRL_Pin, GPIO_PIN_SET);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ir_38k_auto_send, ir_38k_auto_send, ir_38k_auto_send);

void cable_state_update_subscription_callback(const void *msgin) {
    log_i("/cable_state/update");
    uros_cable_state_send_flag = true;
}

void bluetooth_addr_update_subscription_callback(const void *msgin) {
    log_i("/auto_charge/bluetooth/addr/update");
    bluetooth_pub_flag = true;
}

uint8_t fal_charge_get_packing_req(void) {
    return auto_charge_packing_req;
}

uint8_t fal_charge_set_packing_req(uint8_t is_req) {
    auto_charge_packing_req = is_req;
    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} FAL_CHARGE */
/* @} Robot_FAL */
