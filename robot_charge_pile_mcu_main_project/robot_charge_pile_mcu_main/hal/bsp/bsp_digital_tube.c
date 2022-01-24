/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_digital_tube.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:35
 * @Description: 数码管驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "bsp_digital_tube.h"
#include "drv_spi.h"
#include "drv_gpio.h"
#include "fal_charge_auto.h"
#include "task_digital_tube.h"
#include "log.h"
#include "common_def.h"

//数码管片选段码数组
uint8_t sel_seg_code_array[SEL_MAX_INDEX] = {
    DIGITAL_TUBE_0_SEL_SEG_CODE,
    DIGITAL_TUBE_1_SEL_SEG_CODE,
    DIGITAL_TUBE_2_SEL_SEG_CODE,
    DIGITAL_TUBE_3_SEL_SEG_CODE,
};

//数码管显示段码数组
uint8_t display_seg_code_array[DISPLAY_SEG_CODE_MAX_INDEX] = {
    DIGITAL_TUBE_NUM_0_SEG_CODE,       DIGITAL_TUBE_NUM_1_SEG_CODE,
    DIGITAL_TUBE_NUM_2_SEG_CODE,       DIGITAL_TUBE_NUM_3_SEG_CODE,
    DIGITAL_TUBE_NUM_4_SEG_CODE,       DIGITAL_TUBE_NUM_5_SEG_CODE,
    DIGITAL_TUBE_NUM_6_SEG_CODE,       DIGITAL_TUBE_NUM_7_SEG_CODE,
    DIGITAL_TUBE_NUM_8_SEG_CODE,       DIGITAL_TUBE_NUM_9_SEG_CODE,
    DIGITAL_TUBE_NUM_A_SEG_CODE,       DIGITAL_TUBE_NUM_B_SEG_CODE,
    DIGITAL_TUBE_NUM_C_SEG_CODE,       DIGITAL_TUBE_NUM_D_SEG_CODE,
    DIGITAL_TUBE_NUM_E_SEG_CODE,       DIGITAL_TUBE_NUM_F_SEG_CODE,
    DIGITAL_TUBE_DISPLAY_OFF_SEG_CODE,
};

//电量百分比进度条显示段码数组
uint8_t
    progress_bar_level_seg_code_array[PROGRESS_BAR_LEVEL_SEG_CODE_MAX_INDEX] = {
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_0_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_1_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_2_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_3_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_4_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_5_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_6_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_7_SEG_CODE,
        DIGITAL_TUBE_PROGRESS_BAR_LEVEL_8_SEG_CODE,
};

/******************************************************************************
 * @Function: bsp_get_digital_tube_progress_bar_level
 * @Description: 获取电池电量进度条数据索引
 * @Input: battery_percentage：电池电量百分比
 * @Output: None
 * @Return: 电池电量进度条数据索引
 * @Others: None
 * @param {uint8_t} battery_percentage
 *******************************************************************************/
uint8_t bsp_get_digital_tube_progress_bar_level(uint8_t battery_percentage) {
    if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_0) &&
        (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_1)) {
        return PROGRESS_BAR_LEVEL_1_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_1) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_2)) {
        return PROGRESS_BAR_LEVEL_2_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_2) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_3)) {
        return PROGRESS_BAR_LEVEL_3_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_3) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_4)) {
        return PROGRESS_BAR_LEVEL_4_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_4) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_5)) {
        return PROGRESS_BAR_LEVEL_5_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_5) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_6)) {
        return PROGRESS_BAR_LEVEL_6_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_6) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_7)) {
        return PROGRESS_BAR_LEVEL_7_SEG_CODE_INDEX;
    } else if ((battery_percentage > BATTERY_PERCENTAGE_LEVEL_7) &&
               (battery_percentage <= BATTERY_PERCENTAGE_LEVEL_8)) {
        return PROGRESS_BAR_LEVEL_8_SEG_CODE_INDEX;
    } else {
        return PROGRESS_BAR_LEVEL_0_SEG_CODE_INDEX;
    }
}

/******************************************************************************
 * @Function: bsp_digital_tube_send_data
 * @Description: 数码管显示服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_digital_tube_send_data(void) {
    static uint16_t cyclc_display_cnt;
    static uint8_t display_index;
    uint8_t sel_seg_code_index = 0;
    uint8_t display_seg_code_index = 0;
    uint8_t code[2] = {0};
    uint16_t display_data = 0;
    uint8_t diaplay_data_type = 0;

    if (digital_tube_var.display_data_num > 1) {
        cyclc_display_cnt++;

        if (cyclc_display_cnt == 300) {
            cyclc_display_cnt = 0;
            display_index++;
            display_index %= digital_tube_var.display_data_num;
        }
    } else {
        cyclc_display_cnt = 0;
        display_index = 0;
    }

    display_data = digital_tube_var.display_data[display_index];
    diaplay_data_type = digital_tube_var.display_data_type[display_index];

    switch (digital_tube_var.current_status) {
        case DIGITAL_TUBE_0_DISPLAY_EN:
            sel_seg_code_index = SEL_0_INDEX;

            if (diaplay_data_type == FUNCTION_CODE_DISPLAY) {
                display_seg_code_index = (display_data >> 8) & 0x0F;

                code[0] = sel_seg_code_array[sel_seg_code_index];
                code[1] = display_seg_code_array[display_seg_code_index];
            } else {
                display_seg_code_index = display_data / 100;

                code[0] = display_seg_code_index
                              ? sel_seg_code_array[sel_seg_code_index]
                              : sel_seg_code_array[sel_seg_code_index] |
                                    DIGITAL_TUBE_0_OFF_MASK;
                code[1] = display_seg_code_array[display_seg_code_index];
            }

            drv_spi1_send_data(code, sizeof(code));
            digital_tube_var.current_status = DIGITAL_TUBE_1_DISPLAY_EN;
            break;

        case DIGITAL_TUBE_1_DISPLAY_EN:
            sel_seg_code_index = SEL_1_INDEX;

            display_seg_code_index = (display_data & 0xFF) % 100 / 10;

            if (diaplay_data_type == FUNCTION_CODE_DISPLAY) {
                code[0] = sel_seg_code_array[sel_seg_code_index];
                code[1] = display_seg_code_array[display_seg_code_index];
            } else {
                code[0] = ((display_data / 100) || display_seg_code_index)
                              ? sel_seg_code_array[sel_seg_code_index]
                              : (sel_seg_code_array[sel_seg_code_index] |
                                 DIGITAL_TUBE_1_OFF_MASK);
                code[1] = display_seg_code_array[display_seg_code_index];
            }

            drv_spi1_send_data(code, sizeof(code));
            digital_tube_var.current_status = DIGITAL_TUBE_2_DISPLAY_EN;
            break;

        case DIGITAL_TUBE_2_DISPLAY_EN:
            sel_seg_code_index = SEL_2_INDEX;

            display_seg_code_index = (display_data & 0xFF) % 10;
            code[0] = sel_seg_code_array[sel_seg_code_index];
            code[1] = display_seg_code_array[display_seg_code_index];

            if (diaplay_data_type == FUNCTION_CODE_DISPLAY) {
                digital_tube_var.current_status = DIGITAL_TUBE_0_DISPLAY_EN;
            } else {
                digital_tube_var.current_status = DIGITAL_TUBE_3_DISPLAY_EN;
            }

            drv_spi1_send_data(code, sizeof(code));

            break;

        case DIGITAL_TUBE_3_DISPLAY_EN:
            if (diaplay_data_type == BATTERY_DISPLAY) {
                if (SYS_GET_BIT(digital_tube_var.flag,
                                DIGITAL_TUBE_DYNAMIC_DISPLAY_EN_BIT)) {
                    digital_tube_var.progress_bar_cycle_cnt++;

                    if (digital_tube_var.progress_bar_cycle_cnt ==
                        DIGITAL_TUBE_DYNAMIC_DISPLAY_CYCLE_CNT) {
                        digital_tube_var.progress_bar_level++;
                        digital_tube_var.progress_bar_cycle_cnt = 0;
                        digital_tube_var.progress_bar_level =
                            (digital_tube_var.progress_bar_level >
                                     DIGITAL_TUBE_PROGRESS_BAR_NUM
                                 ? bsp_get_digital_tube_progress_bar_level(
                                       display_data)
                                 : digital_tube_var.progress_bar_level);
                    }
                }

                code[0] = sel_seg_code_array[SEL_3_INDEX];
                code[1] =
                    progress_bar_level_seg_code_array[digital_tube_var
                                                          .progress_bar_level];
            } else if (diaplay_data_type == WATER_LEVEL_DISPLAY) {
                code[0] = sel_seg_code_array[SEL_3_INDEX] | 0x10;  //关闭OUTLOOK
                code[1] = progress_bar_level_seg_code_array[0];
            }

            drv_spi1_send_data(code, sizeof(code));
            digital_tube_var.current_status = DIGITAL_TUBE_0_DISPLAY_EN;
            break;

        default:
            LOG_WARNING("Unsupport opt!");
            break;
    }

    DIGITAL_TUBE_LOCK_H();

    for (uint8_t i = 8; i > 0; i--) { __NOP(); }

    DIGITAL_TUBE_LOCK_L();
}

//三个数码管相同显示
void bsp_digital_tube_same_display(uint8_t display_value) {
    static uint8_t sel_seg_code_index = 0;
    uint8_t code[2] = {0};

    sel_seg_code_index++;

    sel_seg_code_index = sel_seg_code_index % (SEL_2_INDEX + 1);

    code[0] = sel_seg_code_array[sel_seg_code_index] | 0x30;
    code[1] = display_value;

    drv_spi1_send_data(code, sizeof(code));

    DIGITAL_TUBE_LOCK_H();

    for (uint8_t i = 8; i > 0; i--) { __NOP(); }

    DIGITAL_TUBE_LOCK_L();
}

/******************************************************************************
 * @Function: bsp_digital_tube_standby_display
 * @Description: 数码管待机显示函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_digital_tube_standby_display(void) {
    bsp_digital_tube_same_display(0xbf);
}

/******************************************************************************
 * @Function: bsp_digital_tube_standby_display
 * @Description: 数码管显示电极压下
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_digital_tube_electrode_down_display(void) {
    bsp_digital_tube_same_display(0xf6);
}
