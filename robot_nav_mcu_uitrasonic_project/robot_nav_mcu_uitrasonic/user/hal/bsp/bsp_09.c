#include "bsp_09.h"
#include "pal_usart4.h"

uint8_t bsp_09_current_channel = 0;
uint8_t bsp_09_distance_val_send_buff[ELMOS09_CHANNEL_NUM * 2];
uint16_t bsp_09_distance_val[ELMOS09_CHANNEL_NUM];
log_ultrasonicl_thres_enum_t log_ultrasonicl_thres =
    LOG_ULTRASONIC_THRES_MIDDLE;
uint8_t digital_ultrasonic_state = ELMOS09_ON;

void bsp_09_channel_select(uint8_t channel) {
    SYS_GET_BIT(channel, 0) ? UITRASONIC_CHANNEL_SEL_2_H()
                            : UITRASONIC_CHANNEL_SEL_2_L();
    SYS_GET_BIT(channel, 1) ? UITRASONIC_CHANNEL_SEL_1_H()
                            : UITRASONIC_CHANNEL_SEL_1_L();
    SYS_GET_BIT(channel, 2) ? UITRASONIC_CHANNEL_SEL_0_H()
                            : UITRASONIC_CHANNEL_SEL_0_L();
}

void bsp_09_CMD() {
    SEGGER_RTT_printf(0, "\n\nCMD\n");
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

// sendA发送指令
void bsp_09_send_A() {
    SEGGER_RTT_printf(0, "\n\nSENDA\n\n");
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
}

// sendb发送指令
void bsp_09_send_B() {
    SEGGER_RTT_printf(0, "\n\nSENDB\n");
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(200);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
}

// sendb接收指令
void bsp_09_receive_B() {
    SEGGER_RTT_printf(0, "\n\nRECEIVEB\n");
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(200);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
}

//超声频率校验
void bsp_09_CAL_PULSES() {
    SEGGER_RTT_printf(0, "\n\nCAL_PULSES\n");
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(200);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
    delay_us(100);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET);
    delay_us(50);
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET);
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

void bsp_09_MEAS_SETUP() {
    bsp_09_CMD();
    uint8_t config[43] = {0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0,
                          0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,
                          1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0};
    for (int i = 0; i < 43; i++) {
        if (config[i] == 0)
            bsp_09_bit0();
        else
            bsp_09_bit1();
    }
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
    // thres8=15
    if (log_ultrasonicl_thres == LOG_ULTRASONIC_THRES_LOW) {
        convertDecimalToBinary(thres2, 15);
        convertDecimalToBinary(thres3, 22);
        convertDecimalToBinary(thres4, 22);
        convertDecimalToBinary(thres5, 22);
        convertDecimalToBinary(thres6, 21);
        convertDecimalToBinary(thres7, 21);
        convertDecimalToBinary(thres8, 15);
        convertDecimalToBinary(thres9, 15);
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

void bsp_09_channel_loop_meas_setup() {
    for (int i = 0; i < 8; i++) {
        bsp_09_channel_select(i);
        bsp_09_MEAS_SETUP();
    }
}

void bsp_09_channel_loop_thres_setup() {
    for (int i = 0; i < 8; i++) {
        bsp_09_channel_select(i);
        bsp_09_THRES_SETUP();
    }
}

//读取第一个低电平+一个高电平时间
uint32_t bsp_09_read_distance() {
    HAL_TIM_Base_Start(&htim6);       //使能TIM6 用于计算收发时间
    __HAL_TIM_SetCounter(&htim6, 0);  // TIM6清零
    uint16_t using_time = 0;

    int i = 0;
    while (1) {
        switch (capture_Cnt) {
            case 0:
                capture_Cnt++;
                __HAL_TIM_SET_COUNTER(&htim5, 0);  //设置计数寄存器的值变为0
                __HAL_TIM_SET_CAPTUREPOLARITY(
                    &htim5, TIM_CHANNEL_1,
                    TIM_ICPOLARITY_FALLING);  //设置为下降沿捕获
                HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);  //启动输入捕获
                break;
            case 3:
                high_time = capture_Buf[1] - capture_Buf[0];  //高电平时间
                high_time_buff[i++] = high_time;
                capture_Cnt = 0;  //清空标志位
                break;
        }

        //若命令发送后15ms未收到回波 显示timeout
        if (__HAL_TIM_GetCounter(&htim6) >= 15000) {
            SEGGER_RTT_printf(0, "timeout\n");
            HAL_TIM_Base_Stop(&htim6);
            capture_Cnt = 0;                            //清空标志位
            HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1);  //停止捕获
            return 0;
        }
        //读取五个低高电平后跳出循环
        if (i == 5)
            break;
    }
    using_time = __HAL_TIM_GetCounter(&htim6);
    HAL_TIM_Base_Stop(&htim6);
    //第一次时间偏移修正
    uint16_t time_delay_fix = high_time_buff[0] - 160;
    uint16_t time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    SEGGER_RTT_printf(0, "using_time:%ldus\n", using_time);
    SEGGER_RTT_printf(0, "low_high_time:%ldus\n", high_time_buff[0]);
    //第二次时间偏移修正
    if (time_delay_fix_distance >= 585) {
        time_delay_fix = high_time_buff[0] - 120;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    //第三次时间偏移修正
    if (time_delay_fix_distance >= 785) {
        time_delay_fix = high_time_buff[0] - 100;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    //第四次时间偏移修正
    if (time_delay_fix_distance >= 950) {
        time_delay_fix = high_time_buff[0] - 70;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    SEGGER_RTT_printf(0, "distance:%ldmm\n", time_delay_fix_distance);
    return time_delay_fix_distance;
}

//用于双探头读取第一个下降沿电平持续时间
uint32_t bsp_09_receive_read() {
    HAL_TIM_Base_Start(&htim6);  //使能TIM6 用于计算开始到第一个下降沿的时间
    __HAL_TIM_SetCounter(&htim6, 0);  // TIM6清零
    uint16_t using_time = 0;
    receive_flag = 1;

    while (1) {
        switch (capture_Cnt) {
            case 0:
                capture_Cnt++;
                __HAL_TIM_SET_COUNTER(&htim5, 0);  //设置计数寄存器的值变为0
                __HAL_TIM_SET_CAPTUREPOLARITY(
                    &htim5, TIM_CHANNEL_1,
                    TIM_ICPOLARITY_FALLING);  //设置为下降沿捕获
                HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);  //启动输入捕获
                break;
        }
        //若命令发送后15ms未收到回波 显示timeout
        if (__HAL_TIM_GetCounter(&htim6) >= 8600) {
            SEGGER_RTT_printf(0, "timeout\n");
            capture_Cnt = 0;  //清空标志位
            receive_flag = 0;
            return 0;
        }
        //读取到一个bit位后跳出循环
        if (receive_flag == 0)
            break;
    }
    capture_Cnt = 0;  //清空标志位
    receive_flag = 0;
    using_time = __HAL_TIM_GetCounter(&htim6);
    SEGGER_RTT_printf(0, "using_time:%ldus\n", using_time);
    return using_time;
}

void bsp_09_report_distance_value(void) {
    uint8_t i = 0;

    for (i = 0; i < ELMOS09_CHANNEL_NUM; i++) {
        bsp_09_distance_val_send_buff[2 * i] =
            (bsp_09_distance_val[i] >> 8) & 0xFF;
        bsp_09_distance_val_send_buff[2 * i + 1] =
            bsp_09_distance_val[i] & 0xFF;
    }

    pal_usart4_send_uitrasonic_distance_msg(bsp_09_distance_val_send_buff,
                                            ELMOS09_CHANNEL_NUM * 2);
}

uint8_t bsp_09_channel_convert(uint8_t channel) {
    uint8_t ret = 0;

    switch (channel) {
        case 1:
            ret = 8;
            break;

        case 2:
            ret = 5;
            break;

        case 3:
            ret = 7;
            break;

        case 4:
            ret = 6;
            break;

        case 5:
            ret = 4;
            break;

        case 6:
            ret = 3;
            break;

        case 7:
            ret = 2;
            break;

        case 8:
            ret = 1;
            break;

        default:
            SEGGER_RTT_printf(0, "Channel invaild!");
            break;
    }

    return ret;
}


uint8_t bsp_09_channel_overturn_convert(uint8_t channel) {
    uint8_t ret = 0;

    switch (channel) {
        case 1:
            ret = 8;
            break;

        case 2:
            ret = 7;
            break;

        case 3:
            ret = 6;
            break;

        case 4:
            ret = 5;
            break;

        case 5:
            ret = 4;
            break;

        case 6:
            ret = 3;
            break;

        case 7:
            ret = 2;
            break;

        case 8:
            ret = 1;
            break;

        default:
            SEGGER_RTT_printf(0, "Channel invaild!");
            break;
    }

    return ret;
}

//单探头收发 安保线序
void bsp_09_distance_detect_server(void) {
    if (digital_ultrasonic_state == ELMOS09_OFF)
        return;
    // bsp_09_THRES_SETUP()；
    //切换通道
    bsp_09_channel_select(bsp_09_current_channel);
    bsp_09_current_channel++;
    //发送命令
    bsp_09_send_B();
    //抓取数据
    bsp_09_distance_val[bsp_09_channel_convert(bsp_09_current_channel) - 1] =
        bsp_09_read_distance();
    // if(bsp_09_distance_val[bsp_09_channel_convert(bsp_09_current_channel)-1]
    // > 1100)
    //   bsp_09_distance_val[bsp_09_channel_convert(bsp_09_current_channel)-1] =
    //   1100;
    SEGGER_RTT_printf(
        0,
        ">>>>>>>>>>>>>>>>>>>>>>>>>>>current_channel:%ld   distance:%ldmm "
        "<<<<<<<<<<<<<<<<<<<<<<<<<<<<    \n",
        bsp_09_channel_convert(bsp_09_current_channel),
        bsp_09_distance_val[bsp_09_channel_convert(bsp_09_current_channel) -
                            1]);
    SEGGER_RTT_printf(0, "%d   %d    %d    \n",
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_0_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_1_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_2_Pin));
    //判断一轮检测是否结束 是上报 否则继续
    if (bsp_09_current_channel == ELMOS09_CHANNEL_NUM) {
        //上报超声数据
        bsp_09_report_distance_value();
        //通道清零
        bsp_09_current_channel = 0;
    }
    delay_ms(10);
}

//双探头超声收发 探头1发 探头2收
void bsp_09_distance_detect_server2(void) {
    //切换通道
    bsp_09_channel_select(1);

    //发送命令

    bsp_09_send_B();

    //切换通道
    bsp_09_channel_select(2);

    //抓取数据
    bsp_09_receive_B();
    uint32_t time_delay = bsp_09_receive_read() + 90;
    uint32_t distance = time_delay * 340 / 1000 / 2;
    SEGGER_RTT_printf(0, " distance:%ldmm <<<<<<<<<<<<<<<<<<<<<<<<<<<<    \n",
                      distance);
    SEGGER_RTT_printf(0, "%d   %d    %d    \n",
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_0_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_1_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_2_Pin));

    delay_ms(15);
}

//单探头收发 正常遍历线序 当前清洁使用的遍历顺序
void bsp_09_distance_detect_server3(void) {
    //切换通道
    bsp_09_channel_select(bsp_09_current_channel);
    bsp_09_current_channel++;
    //发送命令
    bsp_09_send_B();
    //抓取数据
    bsp_09_distance_val[bsp_09_channel_overturn_convert(bsp_09_current_channel) -
                            1] = bsp_09_read_distance();
    // if(bsp_09_distance_val[bsp_09_current_channel-1] > 1100)
    //   bsp_09_distance_val[bsp_09_current_channel-1]= 1100;
    SEGGER_RTT_printf(0,
                      ">>>>>>>>>>>>>>>>>>>>>>>>>>>current_channel:%ld   "
                      "distance:%ldmm <<<<<<<<<<<<<<<<<<<<<<<<<<<<    \n",
                      bsp_09_current_channel - 1,
                      bsp_09_distance_val[bsp_09_current_channel - 1]);
    SEGGER_RTT_printf(0, "%d   %d    %d    \n",
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_0_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_1_Pin),
                      HAL_GPIO_ReadPin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port,
                                       UITRASONIC_CHANNEL_SEL_2_Pin));

    //判断一轮检测是否结束 是上报 否则继续
    if (bsp_09_current_channel == ELMOS09_CHANNEL_NUM) {
        //上报超声数据
        bsp_09_report_distance_value();
        //通道清零
        bsp_09_current_channel = 0;
        //指示灯翻转
        HAL_GPIO_TogglePin(STATE_LED_GPIO_Port, STATE_LED_Pin);
    }
    delay_ms(10);
}
