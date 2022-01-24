/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_digital_tube.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:35
 * @Description: 数码管驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BSP_DIGITAL_TUBE_H__
#define __BSP_DIGITAL_TUBE_H__

#include <stdint.h>

//电池电量进度条个数
#ifndef DIGITAL_TUBE_PROGRESS_BAR_NUM
#define DIGITAL_TUBE_PROGRESS_BAR_NUM 8
#endif

//数码管类型：0:共阳极  1：共阴极
#ifndef DIGITAL_TUBE_TYPE
#define DIGITAL_TUBE_TYPE 0
#endif

#if DIGITAL_TUBE_TYPE
#define DIGITAL_TUBE_DISPLAY_OFF_MASK 0x0F  //熄灭数码管的掩码
#define DIGITAL_TUBE_0_OFF_MASK 0xFE        //熄灭第一个数码管
#define DIGITAL_TUBE_1_OFF_MASK 0xFD        //熄灭第二个数码管
#define DIGITAL_TUBE_2_OFF_MASK 0xFB        //熄灭第三个数码管
#define DIGITAL_TUBE_3_OFF_MASK 0xF7        //熄灭第四个数码管

#define DIGITAL_TUBE_0_SEL_SEG_CODE \
    0x31  //使能第1个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_1_SEL_SEG_CODE \
    0x32  //使能第2个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_2_SEL_SEG_CODE \
    0x34  //使能第3个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_3_SEL_SEG_CODE \
    0x38  //使能第4个数码管显示，同时点亮%、OUTLOOK

#define DIGITAL_TUBE_NUM_0_SEG_CODE 0x3F        //数码管显示0的段码
#define DIGITAL_TUBE_NUM_1_SEG_CODE 0x06        //数码管显示1的段码
#define DIGITAL_TUBE_NUM_2_SEG_CODE 0x5B        //数码管显示2的段码
#define DIGITAL_TUBE_NUM_3_SEG_CODE 0x4F        //数码管显示3的段码
#define DIGITAL_TUBE_NUM_4_SEG_CODE 0x66        //数码管显示4的段码
#define DIGITAL_TUBE_NUM_5_SEG_CODE 0x6D        //数码管显示5的段码
#define DIGITAL_TUBE_NUM_6_SEG_CODE 0x7D        //数码管显示6的段码
#define DIGITAL_TUBE_NUM_7_SEG_CODE 0x07        //数码管显示7的段码
#define DIGITAL_TUBE_NUM_8_SEG_CODE 0x7F        //数码管显示8的段码
#define DIGITAL_TUBE_NUM_9_SEG_CODE 0x6F        //数码管显示9的段码
#define DIGITAL_TUBE_NUM_A_SEG_CODE 0x77        //数码管显示A的段码
#define DIGITAL_TUBE_NUM_B_SEG_CODE 0x7C        //数码管显示B的段码
#define DIGITAL_TUBE_NUM_C_SEG_CODE 0x39        //数码管显示C的段码
#define DIGITAL_TUBE_NUM_D_SEG_CODE 0x5E        //数码管显示D的段码
#define DIGITAL_TUBE_NUM_E_SEG_CODE 0x79        //数码管显示E的段码
#define DIGITAL_TUBE_NUM_F_SEG_CODE 0x71        //数码管显示F的段码
#define DIGITAL_TUBE_DISPLAY_OFF_SEG_CODE 0x00  //数码管全灭的段码

#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_0_SEG_CODE 0x00  //电量为0%
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_1_SEG_CODE 0x80  //电量为(0~12.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_2_SEG_CODE 0xC0  //电量为(12.5%~25%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_3_SEG_CODE 0xE0  //电量为(25%~37.5]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_4_SEG_CODE 0xF0  //电量为(37.5%~50%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_5_SEG_CODE 0xF8  //电量为(50%~62.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_6_SEG_CODE 0xFC  //电量为(62.5%~75%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_7_SEG_CODE 0xFE  //电量为(75%~87.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_8_SEG_CODE 0xFF  //电量为(87.5%~100%]

#else

#define DIGITAL_TUBE_DISPLAY_OFF_MASK 0xF0  //熄灭数码管的掩码
#define DIGITAL_TUBE_0_OFF_MASK 0x01        //熄灭第一个数码管
#define DIGITAL_TUBE_1_OFF_MASK 0x02        //熄灭第二个数码管
#define DIGITAL_TUBE_2_OFF_MASK 0x04        //熄灭第三个数码管
#define DIGITAL_TUBE_3_OFF_MASK 0x08        //熄灭第四个数码管

#define DIGITAL_TUBE_0_SEL_SEG_CODE \
    0xCE  //使能第1个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_1_SEL_SEG_CODE \
    0xCD  //使能第2个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_2_SEL_SEG_CODE \
    0xCB  //使能第3个数码管显示，同时点亮%、OUTLOOK
#define DIGITAL_TUBE_3_SEL_SEG_CODE \
    0xC7  //使能第4个数码管显示，同时点亮%、OUTLOOK

#define DIGITAL_TUBE_NUM_0_SEG_CODE 0xC0        //数码管显示0的段码
#define DIGITAL_TUBE_NUM_1_SEG_CODE 0xF9        //数码管显示1的段码
#define DIGITAL_TUBE_NUM_2_SEG_CODE 0xA4        //数码管显示2的段码
#define DIGITAL_TUBE_NUM_3_SEG_CODE 0xB0        //数码管显示3的段码
#define DIGITAL_TUBE_NUM_4_SEG_CODE 0x99        //数码管显示4的段码
#define DIGITAL_TUBE_NUM_5_SEG_CODE 0x92        //数码管显示5的段码
#define DIGITAL_TUBE_NUM_6_SEG_CODE 0x82        //数码管显示6的段码
#define DIGITAL_TUBE_NUM_7_SEG_CODE 0xF8        //数码管显示7的段码
#define DIGITAL_TUBE_NUM_8_SEG_CODE 0x80        //数码管显示8的段码
#define DIGITAL_TUBE_NUM_9_SEG_CODE 0x90        //数码管显示9的段码
#define DIGITAL_TUBE_NUM_A_SEG_CODE 0x88        //数码管显示A的段码
#define DIGITAL_TUBE_NUM_B_SEG_CODE 0x83        //数码管显示B的段码
#define DIGITAL_TUBE_NUM_C_SEG_CODE 0xC6        //数码管显示C的段码
#define DIGITAL_TUBE_NUM_D_SEG_CODE 0xA1        //数码管显示D的段码
#define DIGITAL_TUBE_NUM_E_SEG_CODE 0x86        //数码管显示E的段码
#define DIGITAL_TUBE_NUM_F_SEG_CODE 0x8E        //数码管显示F的段码
#define DIGITAL_TUBE_DISPLAY_OFF_SEG_CODE 0xFF  //数码管全灭的段码

#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_0_SEG_CODE 0xFF  //电量为0%
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_1_SEG_CODE 0x7F  //电量为(0~12.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_2_SEG_CODE 0x3F  //电量为(12.5%~25%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_3_SEG_CODE 0x1F  //电量为(25%~37.5]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_4_SEG_CODE 0x0F  //电量为(37.5%~50%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_5_SEG_CODE 0x07  //电量为(50%~62.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_6_SEG_CODE 0x03  //电量为(62.5%~75%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_7_SEG_CODE 0x01  //电量为(75%~87.5%]
#define DIGITAL_TUBE_PROGRESS_BAR_LEVEL_8_SEG_CODE 0x00  //电量为(87.5%~100%]
#endif

typedef enum {
    SEL_0_INDEX = 0,
    SEL_1_INDEX,
    SEL_2_INDEX,
    SEL_3_INDEX,
    SEL_MAX_INDEX,
} digital_tube_sel_seg_code_index_enum_t;

typedef enum {
    NUM_0_SEG_CODE_INDEX = 0,
    NUM_1_SEG_CODE_INDEX,
    NUM_2_SEG_CODE_INDEX,
    NUM_3_SEG_CODE_INDEX,
    NUM_4_SEG_CODE_INDEX,
    NUM_5_SEG_CODE_INDEX,
    NUM_6_SEG_CODE_INDEX,
    NUM_7_SEG_CODE_INDEX,
    NUM_8_SEG_CODE_INDEX,
    NUM_9_SEG_CODE_INDEX,
    NUM_A_SEG_CODE_INDEX,
    NUM_B_SEG_CODE_INDEX,
    NUM_C_SEG_CODE_INDEX,
    NUM_D_SEG_CODE_INDEX,
    NUM_E_SEG_CODE_INDEX,
    NUM_F_SEG_CODE_INDEX,
    DISPLAY_OFF_SEG_CODE_INDEX,
    DISPLAY_SEG_CODE_MAX_INDEX,
} display_code_index_enum_t;

typedef enum {
    PROGRESS_BAR_LEVEL_0_SEG_CODE_INDEX = 0,
    PROGRESS_BAR_LEVEL_1_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_2_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_3_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_4_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_5_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_6_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_7_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_8_SEG_CODE_INDEX,
    PROGRESS_BAR_LEVEL_SEG_CODE_MAX_INDEX,
} digital_tube_progress_bar_level_seg_code_index_enum_t;

uint8_t bsp_get_digital_tube_progress_bar_level(uint8_t battery_percentage);
void bsp_digital_tube_send_data(void);
void bsp_digital_tube_standby_display(void);
void bsp_digital_tube_electrode_down_display(void);

#endif
