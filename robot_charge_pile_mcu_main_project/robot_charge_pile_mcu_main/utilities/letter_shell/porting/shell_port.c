/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell_port.h"

Shell shell;
char shellBuffer[512];
uint8_t letter_shell_recv_buff = 0;

/**
 * @brief 用户shell写
 *
 * @param data 数据
 */
void userShellWrite(char data) {
    serialTransmit(&huart1, (uint8_t *) &data, 1, 1000);
}

#if 1
/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @return char 状态
 */
signed char userShellRead(char *data) {
    if (serialReceive(&huart1, (uint8_t *) data, 1, 0) == 1) {
        return 0;
    } else {
        return -1;
    }
}
#endif

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void) {
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);
}
