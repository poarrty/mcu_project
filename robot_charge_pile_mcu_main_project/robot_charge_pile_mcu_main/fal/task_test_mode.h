/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_test_mode.h
 * @Author: LinZhongXing(linzhongxing@cvte.com)
 * @Version: V1.0
 * @Date: 2021-07-21 19:05:00
 * @Description: 测试模式任务入口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_TEST_MODE_H__
#define __TASK_TEST_MODE_H__

#include <stdint.h>

typedef enum {
    TEST_ON = 0,
    TEST_OFF = 1,
} test_mode_enum_t;

typedef enum {
    BUTTON_FORCE_CHARGE = 0,
    BUTTON_STOP_CHARGE = 1,
    BUTTON_TEST_MODE_ENTER =
        (1 << BUTTON_FORCE_CHARGE) | (1 << BUTTON_STOP_CHARGE),
} enter_test_mode_enum_t;

void set_test_mode(void);

#endif
