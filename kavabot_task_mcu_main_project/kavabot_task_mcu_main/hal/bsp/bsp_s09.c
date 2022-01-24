/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_s09.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 16:44:58
 * @Description: 超声传感器驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "bsp_s09.h"
#include "drv_gpio.h"
#include "pal_usart3.h"
#include "common_def.h"
#include "log.h"
#include "tim.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

s09_var_stu_t s09_var;
log_ultrasonicl_thres_enum_t log_ultrasonicl_thres = LOG_ULTRASONIC_THRES_LOW;
/******************************************************************************
 * @Function: bsp_s09_init
 * @Description: 超声初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_s09_init(void) {
    HAL_GPIO_WritePin(UITRASONIC_PWR_EN_GPIO_Port, UITRASONIC_PWR_EN_Pin,
                      GPIO_PIN_SET);    //超声上电
    HAL_Delay(5000);                    //确保超声上电完成
    HAL_NVIC_DisableIRQ(USART3_IRQn);   //排除串口接受中断干扰
    bsp_09_channel_loop_thres_setup();  //设置阈值
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    memset(&s09_var, 0, sizeof(s09_var_stu_t));
    HAL_TIM_Base_Start_IT(&htim4);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
}
/******************************************************************************
 * @Function:
 * @Description: 阈值配置
 * @Input:
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_09_CMD() {
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(250);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
}

void bsp_09_bit0() {
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
}

void bsp_09_bit1() {
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(100);
}

int bsp_09_parity(int *p, int num) {
    unsigned int v = num;
    int parity = 0;  //初始判断标记
    while (v) {
        if (*p == 1) {
            parity = !parity;
        }
        v--;
        p++;
    }
    return parity;
}

void convertDecimalToBinary(int *thres_array, int thres_num) {
    int remainder, step = 1;

    while (thres_num != 0) {
        remainder = thres_num % 2;
        thres_num /= 2;
        step++;
        thres_array[6 - step] = remainder;
    }
    return;
}

//超声阈值设置 阈值曲线整体设置得越大 超声对应的最大角度和最大距离越小
void bsp_09_THRES_SETUP() {
    int thres2[5] = {0};
    int thres3[5] = {0};
    int thres4[5] = {0};
    int thres5[5] = {0};
    int thres6[5] = {0};
    int thres7[5] = {0};
    int thres8[5] = {0};
    int thres9[5] = {0};
    int thres10[5] = {0};
    int thres11[5] = {0};
    int thres12[5] = {0};
    int thres13[5] = {0};
    // thres2=15 thres3=22 thres4=22 thres5=22 thres6=21 thres7=21 thres8=15
    // thres8=15 任务板超声阈值
    if (log_ultrasonicl_thres == LOG_ULTRASONIC_THRES_LOW) {
        convertDecimalToBinary(thres2, 22);
        convertDecimalToBinary(thres3, 27);
        convertDecimalToBinary(thres4, 27);
        convertDecimalToBinary(thres5, 27);
        convertDecimalToBinary(thres6, 27);
        convertDecimalToBinary(thres7, 25);
        convertDecimalToBinary(thres8, 20);
        convertDecimalToBinary(thres9, 20);
    }
    // thres2=17 thres3=24 thres4=24 thres5=24 thres6=23 thres7=23 thres8=17
    // thres8=17 清洁当前使用fov
    else if (log_ultrasonicl_thres == LOG_ULTRASONIC_THRES_MIDDLE) {
        convertDecimalToBinary(thres2, 21);
        convertDecimalToBinary(thres3, 27);
        convertDecimalToBinary(thres4, 27);
        convertDecimalToBinary(thres5, 25);
        convertDecimalToBinary(thres6, 25);
        convertDecimalToBinary(thres7, 25);
        convertDecimalToBinary(thres8, 21);
        convertDecimalToBinary(thres9, 21);
    }
    // thres2=19 thres3=26 thres4=26 thres5=26 thres6=25 thres7=25 thres8=19
    // thres8=19
    else if (log_ultrasonicl_thres == LOG_ULTRASONIC_THRES_HIGH) {
        convertDecimalToBinary(thres2, 19);
        convertDecimalToBinary(thres3, 26);
        convertDecimalToBinary(thres4, 26);
        convertDecimalToBinary(thres5, 26);
        convertDecimalToBinary(thres6, 25);
        convertDecimalToBinary(thres7, 25);
        convertDecimalToBinary(thres8, 19);
        convertDecimalToBinary(thres9, 19);
    }
    LOG_WARNING("LOG_ULTRASONIC_THRES = %d", log_ultrasonicl_thres);
    int other[9] = {0, 0, 1, 0, 0, 1, 0, 0, 1};
    int myconfig[78] = {0};
    for (int num = 0; num < 78; num++) {
        if (num < 4)
            myconfig[num] = 0;
        if (num >= 4 && num < 9)
            myconfig[num] = thres2[num - 4];
        if (num >= 9 && num < 14)
            myconfig[num] = thres3[num - 9];
        if (num >= 14 && num < 19)
            myconfig[num] = thres4[num - 14];
        if (num >= 19 && num < 24)
            myconfig[num] = thres5[num - 19];
        if (num >= 24 && num < 29)
            myconfig[num] = thres6[num - 24];
        if (num >= 29 && num < 34)
            myconfig[num] = thres7[num - 29];
        if (num >= 34 && num < 39)
            myconfig[num] = thres8[num - 34];
        if (num >= 39 && num < 44)
            myconfig[num] = thres9[num - 39];
        if (num >= 44 && num < 49)
            myconfig[num] = thres10[num - 44];
        if (num >= 49 && num < 54)
            myconfig[num] = thres11[num - 49];
        if (num >= 54 && num < 59)
            myconfig[num] = thres12[num - 54];
        if (num >= 59 && num < 64)
            myconfig[num] = thres13[num - 59];
        if (num >= 64 && num < 73)
            myconfig[num] = other[num - 64];
        if (num == 73)
            myconfig[73] = bsp_09_parity(myconfig + 59, 14);
        if (num == 74)
            myconfig[74] = bsp_09_parity(myconfig + 45, 14);
        if (num == 75)
            myconfig[75] = bsp_09_parity(myconfig + 31, 14);
        if (num == 76)
            myconfig[76] = bsp_09_parity(myconfig + 17, 14);
        if (num == 77)
            myconfig[77] = bsp_09_parity(myconfig + 4, 13);
    }
    bsp_09_CMD();
    // uint8_t config[78]={0,0,0,0 ,0,1,1,1,1 ,1,1,1,0,0 ,1,1,1,0,0 ,1,1,1,0,0
    // ,1,1,0,1,1 ,1,1,0,1,1 ,1,1,0,0,0 ,1,1,0,0,0 ,0,0,0,0,0 ,0,0,0,0,0
    // ,0,0,0,0,0 ,0,0,0,0,0 ,0,0 ,1,0 ,0,1 ,0 ,0,1 ,1,0,0,1,0};
    //前四位是命令固定的0000 中间若干个五位数据为寄存器数据 最后五位奇偶校验位
    // uint8_t config[78]={0,0,0,0 ,0,1,1,1,1 ,1,0,1,1,0 ,1,0,1,1,0 ,1,0,1,1,0
    // ,1,0,1,0,1 ,1,0,1,0,1 ,0,1,1,1,1 ,0,1,1,1,1 ,0,0,0,0,0 ,0,0,0,0,0
    // ,0,0,0,0,0 ,0,0,0,0,0 ,0,0 ,1,0 ,0,1 ,0 ,0,1 ,1,0,0,0,1};
    for (int i = 0; i < 78; i++) {
        if (myconfig[i] == 0)
            bsp_09_bit0();
        else
            bsp_09_bit1();
    }
}

void bsp_09_channel_loop_thres_setup() {
    for (int i = 0; i < 2; i++) {
        bsp_s09_channel_select(i);
        bsp_09_THRES_SETUP();
    }
}
/******************************************************************************
 * @Function: bsp_s09_channel_select
 * @Description: 超声通道选择
 * @Input: channel：选择的通道号：0 ~ 7
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} channel
 *******************************************************************************/
void bsp_s09_channel_select(uint8_t channel) {
    SYS_GET_BIT(channel, 2) ? UITRASONIC_CHANNEL_SEL_2_H()
                            : UITRASONIC_CHANNEL_SEL_2_L();
    SYS_GET_BIT(channel, 1) ? UITRASONIC_CHANNEL_SEL_1_H()
                            : UITRASONIC_CHANNEL_SEL_1_L();
    SYS_GET_BIT(channel, 0) ? UITRASONIC_CHANNEL_SEL_0_H()
                            : UITRASONIC_CHANNEL_SEL_0_L();
}

/******************************************************************************
 * @Function: bsp_s09_report_distance_value
 * @Description: 上报超声测距结果
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_s09_report_distance_value(void) {
    s09_var.distance_val_send_buff[0] = (s09_var.distance_val[1] >> 8) & 0xFF;
    s09_var.distance_val_send_buff[1] = s09_var.distance_val[1] & 0xFF;
    s09_var.distance_val_send_buff[2] = (s09_var.distance_val[0] >> 8) & 0xFF;
    s09_var.distance_val_send_buff[3] = s09_var.distance_val[0] & 0xFF;

    pal_usart3_send_uitrasonic_distance_msg(s09_var.distance_val_send_buff,
                                            UITRASONIC_CHANNEL_NUM * 2);
}

/******************************************************************************
 * @Function: bsp_s09_distance_detect_server
 * @Description: 超声距离检测服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_s09_distance_detect_server(void) {
    bsp_s09_channel_select(s09_var.current_channel);
    s09_var.current_channel++;

    //发送请求
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    UITRASONIC_40KHZ_PULSE_OUTPUT_H();
    s09_var.send_pulse_status = 0;

    //等待捕获结束或者超时
    while (!SYS_GET_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_FINISH_BIT)) {
        if (SYS_GET_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_TIMEOUT_BIT)) {
            SYS_CLR_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_TIMEOUT_BIT);
            s09_var.distance_val[s09_var.current_channel - 1] = 0;
            break;
        }
    }

    //计算距离值
    if (SYS_GET_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_FINISH_BIT)) {
        s09_var.fix_time = s09_var.echo_time[0] - 160;
        s09_var.fix_distance = s09_var.fix_time * 340 / 1000 / 2;

        if (s09_var.fix_distance >= 585) {
            s09_var.fix_time = s09_var.echo_time[0] - 120;
            s09_var.fix_distance = s09_var.fix_time * 340 / 1000 / 2;
        }

        if (s09_var.fix_distance >= 785) {
            s09_var.fix_time = s09_var.echo_time[0] - 100;
            s09_var.fix_distance = s09_var.fix_time * 340 / 1000 / 2;
        }

        if (s09_var.fix_distance >= 950) {
            s09_var.fix_time = s09_var.echo_time[0] - 70;
            s09_var.fix_distance = s09_var.fix_time * 340 / 1000 / 2;
        }

        s09_var.distance_val[s09_var.current_channel - 1] =
            s09_var.fix_distance;

        SYS_CLR_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_FINISH_BIT);
    }

    LOG_DEBUG("channel:%d,  time:%4d,  distance:%4d", s09_var.current_channel,
              s09_var.echo_time[0],
              s09_var.distance_val[s09_var.current_channel - 1]);

    //一轮检测完上报
    if (s09_var.current_channel == UITRASONIC_CHANNEL_NUM) {
        bsp_s09_report_distance_value();
        SYS_CLR_BIT(s09_var.flag, UITRASONIC_DETECT_EN_BIT);
        s09_var.current_channel = 0;
        MCU_LED_TOGGLE();
    }
}

/******************************************************************************
 * @Function: read_uitrasonic_data
 * @Description: 获取超声数据
 * @Input: register_val：存放读取数据的地址
 *         len：要读取得数据长度
 *         index：起始索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint8_t} len
 * @param {uint8_t} index
 *******************************************************************************/
void read_uitrasonic_data(uint8_t *register_val, uint16_t len, uint16_t index) {
    uint8_t buff[10] = {0};

    buff[0] = s09_var.distance_val[1] >> 8;
    buff[1] = s09_var.distance_val[1] & 0xFF;
    buff[2] = s09_var.distance_val[0] >> 8;
    buff[3] = s09_var.distance_val[0] & 0xFF;

#if __ABNORMAL_TEST_EN__
    buff[8] |= (1 << 0);
    buff[8] |= (1 << 1);
#else
    if ((s09_var.distance_val[0] < 0) ||
        (s09_var.distance_val[0] > UITRASONIC_MAX_DISTANCE)) {
        buff[8] |= (1 << 0);
    }

    if ((s09_var.distance_val[1] < 0) ||
        (s09_var.distance_val[1] > UITRASONIC_MAX_DISTANCE)) {
        buff[8] |= (1 << 1);
    }
#endif

    LOG_DEBUG_ARRAY(buff, ARRAY_SIZE(buff));

    while (len--) { *register_val++ = buff[index++]; }
}
