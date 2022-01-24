#include "sys_exc.h"
#include "stdbool.h"
#include "shell.h"
#include "cmsis_os.h"
#include "sys_paras.h"
#include "stdio.h"
#include "string.h"

osMutexId_t mutex_sys_exc = NULL;

static uint8_t sys_exc[EXC_NUM / 8] = {0x00};

const osMutexAttr_t mutex_sys_exc_attr = {
    "mutex_sys_exc",                        // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

void sys_exc_init(void) {
    memset(sys_exc, 0, sizeof(sys_exc));
    mutex_sys_exc = osMutexNew(&mutex_sys_exc_attr);
}

void sys_exc_set(uint8_t item) {
    uint8_t i, j;

    if (mutex_sys_exc == NULL) {
        return;
    }

    if (sys_paras.sys_exc_detect_enable == 0 && item != EXC65_EMERG) {
        return;
    }

    if (item == 0 || item > EXC_NUM) {
        return;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    osMutexAcquire(mutex_sys_exc, osWaitForever);
    // sys_exc[i] |= 0x01<<j;
    sys_exc[i] |= (0x01 << j) & sys_paras.sys_exc_mask[i];
    osMutexRelease(mutex_sys_exc);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), errorSet, sys_exc_set, sys_exc_set);

void sys_exc_clear(uint8_t item) {
    uint8_t i, j;

    if (mutex_sys_exc == NULL) {
        return;
    }

    if (sys_paras.sys_exc_detect_enable == 0 && item != EXC65_EMERG) {
        return;
    }

    if (item == 0 || item > EXC_NUM) {
        return;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    osMutexAcquire(mutex_sys_exc, osWaitForever);
    sys_exc[i] &= ~(0x01 << j);
    osMutexRelease(mutex_sys_exc);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), errorClr, sys_exc_clear, sys_exc_clear);

void sys_exc_clear_all(void) {
    osMutexAcquire(mutex_sys_exc, osWaitForever);
    memset(sys_exc, 0, sizeof(sys_exc));
    osMutexRelease(mutex_sys_exc);
}

bool sys_exc_get(uint8_t item) {
    bool    val;
    uint8_t i, j;

    if (mutex_sys_exc == NULL) {
        return false;
    }

    if (sys_paras.sys_exc_detect_enable == 0 && item != EXC65_EMERG) {
        return false;
    }

    if (item == 0 || item > EXC_NUM) {
        return false;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    osMutexAcquire(mutex_sys_exc, osWaitForever);
    val = sys_exc[i] & (0x01 << j);
    osMutexRelease(mutex_sys_exc);

    return val;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), errorGet, sys_exc_get, sys_exc_get);

void sys_exc_groud_set(uint8_t index, uint16_t value) {
    osMutexAcquire(mutex_sys_exc, osWaitForever);

    sys_exc[index * 2]     = value & 0XFF;
    sys_exc[index * 2 + 1] = (value >> 8) & 0XFF;

    ///< 屏蔽驱动器总错误，待与驱动器方调试完成后去除屏蔽
    sys_exc_clear(EXC15_MOTOR_ERR_FLAG);

    osMutexRelease(mutex_sys_exc);
}

uint16_t sys_exc_groud_get(uint8_t index) {
    uint16_t ret;

    osMutexAcquire(mutex_sys_exc, osWaitForever);

    ret = sys_exc[index * 2 + 1];
    ret <<= 8;
    ret |= sys_exc[index * 2];

    osMutexRelease(mutex_sys_exc);

    return ret;
}

uint8_t sys_exc_show(void) {
    uint8_t i;

    printf("sys_exc_stat                : ");

    // printf("                              ");
    for (i = 1; i <= EXC_NUM; i++) {
        if (exc_get_item(sys_exc, i) == 0) {
            printf("\033[1;31;40m[%02d:%d]\033[0m ", i, exc_get_item(sys_exc, i));
        } else {
            printf("[%02d:%d] ", i, exc_get_item(sys_exc, i));
        }

        if (i % 16 == 0) {
            printf("\r\n");
            printf("                              ");
        }
    }

    printf("\r\n");

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys_exc_show, sys_exc_show, sys_exc_show);

bool is_sys_exc_none(void) {
    uint8_t i;

    for (i = 0; i < EXC_NUM / 8; i++) {
        if (sys_exc[i] != 0X00) {
            return false;
        }
    }

    return true;
}

bool is_sys_danger_exc_detected(void) {
    if (sys_exc_get(EXC05_MOTOR1L_LOCK) || sys_exc_get(EXC06_MOTOR1R_LOCK) || sys_exc_get(EXC33_MOTOR_COM_TO) ||
        sys_exc_get(EXC35_NVG_SOC_COM_TO) || sys_exc_get(EXC65_EMERG) || sys_exc_get(EXC66_CLIFF) || sys_exc_get(EXC67_CRASH) ||
        sys_exc_get(EXC68_FLOOD) || sys_exc_get(EXC69_RAIN) || sys_exc_get(EXC70_LOW_POWER)) {
        return true;
    } else {
        return false;
    }
}

bool exc_get_item(uint8_t *exc, uint8_t item) {
    bool    val;
    uint8_t i, j;

    if (item == 0 || item > EXC_NUM) {
        return false;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    val = exc[i] & (0x01 << j);

    return val;
}

void exc_set_item(uint8_t *exc, uint8_t item) {
    uint8_t i, j;

    if (item == 0 || item > EXC_NUM) {
        return;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    exc[i] |= (0x01 << j);
}

void exc_clear_item(uint8_t *exc, uint8_t item) {
    uint8_t i, j;

    if (item == 0 || item > EXC_NUM) {
        return;
    } else {
        item -= 1;
    }

    i = item / 8;
    j = item % 8;

    exc[i] &= ~(0x01 << j);
}
