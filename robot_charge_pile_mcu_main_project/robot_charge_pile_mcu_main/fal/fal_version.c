/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_version.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:44:26
 * @Description: 版本信息文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_version.h"
#include "log.h"
#include "shell.h"
#include "main.h"

uint8_t blue_address[6];
const char at_cmd[] = "AT+ADDR?\r\n";
const char at_cmd_resp[] = "+ADDR:";
int fal_bluetooth_init(void) {
    int result = -1;
    BT_POWER_ON;
    HAL_Delay(2000);  //等待蓝牙模块开机,耗时约1秒
    HAL_GPIO_WritePin(WIFI_PWR_EN_GPIO_Port, WIFI_PWR_EN_Pin, SET);
    HAL_Delay(100);

    HAL_UART_Transmit(&huart3, (uint8_t *) at_cmd, strlen(at_cmd), 100);
    uint8_t rx_buff_temp[30] = {0};
    huart3.Instance->DR;

    HAL_UART_Receive(&huart3, rx_buff_temp, sizeof(rx_buff_temp), 1000);
    rx_buff_temp[29] = '\0';
    do {
        printf("receive :%s\r\n", rx_buff_temp);

        char *rx_buff = strstr(rx_buff_temp, at_cmd_resp);  //去除错误数据
        if (rx_buff == NULL) {
            printf("error blue address\r\n");
            break;
        }

        uint8_t buff_index = 6;
        uint8_t buff_index_max = buff_index + 20;
        uint8_t data_index = 0;
        uint8_t data_buff[12] = {0};
        while ((rx_buff[buff_index] != '\r') &&
               (buff_index <= buff_index_max)) {
            if ((rx_buff[buff_index] >= 'a') && (rx_buff[buff_index] <= 'f')) {
                data_buff[data_index++] = rx_buff[buff_index] - 'a' + 10;
            } else if ((rx_buff[buff_index] >= 'A') &&
                       (rx_buff[buff_index] <= 'F')) {
                data_buff[data_index++] = rx_buff[buff_index] - 'A' + 10;
            } else if ((rx_buff[buff_index] >= '0') &&
                       (rx_buff[buff_index] <= '9')) {
                data_buff[data_index++] = rx_buff[buff_index] - '0';
            }

            if (data_index > 12) {
                printf("blue address parse count more than 12!\r\n");
                break;
            }

            buff_index += 1;
        }

        if (data_index != 12) {
            printf("blue address parse error!\r\n");
            break;
        }

        for (size_t i = 0; i < 6; i++) {
            blue_address[i] = data_buff[i * 2] * 16 + data_buff[i * 2 + 1];
        }

        result = 0;
    } while (0);
    BT_POWER_OFF;
    HAL_GPIO_WritePin(WIFI_PWR_EN_GPIO_Port, WIFI_PWR_EN_Pin, RESET);
    HAL_Delay(500);
    return result;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 fal_bluetooth_init, fal_bluetooth_init, fal_bluetooth_init);

void fal_bluetooth_en(uint8_t state) {
    HAL_GPIO_WritePin(WIFI_PWR_EN_GPIO_Port, WIFI_PWR_EN_Pin,
                      state ? SET : RESET);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 fal_bluetooth_en, fal_bluetooth_en, fal_bluetooth_en);

void fal_bluetooth_disc(void) {
    const char at_cmd_disc[] = "AT+DISC\r\n";

    HAL_GPIO_WritePin(WIFI_PWR_EN_GPIO_Port, WIFI_PWR_EN_Pin, SET);
    HAL_Delay(200);
    HAL_UART_Transmit(&huart3, (uint8_t *) at_cmd_disc, strlen(at_cmd_disc),
                      100);
    HAL_Delay(200);

    HAL_GPIO_WritePin(WIFI_PWR_EN_GPIO_Port, WIFI_PWR_EN_Pin, RESET);

    LOG_DEBUG("Bluetooth diconnect.");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 fal_bluetooth_disc, fal_bluetooth_disc, fal_bluetooth_disc);

/******************************************************************************
 * @Function: fal_version_init
 * @Description: 版本初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_init(void) {
    fal_version_info_display();
    HAL_Delay(1500);
    for (size_t i = 0; i < 3; i++) {
        if (fal_bluetooth_init() == 0) {
            break;
        }
    }
}

/******************************************************************************
 * @Function: fal_version_info_display
 * @Description: 版本信息显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_info_display(void) {
    LOG_CUSTOM(INFO_COLOR "\r\n[VERSION]:%s_%s_%s_%s\r\n\r\n" COLOR_END,
               CURRENT_BRANCH, COMMIT_ID, BUILD_TIME, SOFTWARE_VERSION);
}
