#include "usart.h"
#include "app.h"
#include "SEGGER_RTT.h"
#include "log.h"
#include "ota.h"
#include "crc16.h"
#include "md5.h"
#include "rtt_fal.h"
#include "drv_usart.h"
usart_rx_info_stu_t usart4;

extern uint16_t calculate_buff_checksum(uint8_t *buff, uint16_t len);

usart_tx_info_stu_t tx_version;
ota_stu_t ota;
uint8_t ota_start = 0;

void ota_send_version(uint16_t frq) {
    static uint16_t current_times = 0;
    if (++current_times < frq) {
        return;
    } else {
        current_times = 0;
    }

    uint16_t version_len = strlen(SOFTWARE_VERSION);
    tx_version.tx_buff[0] = 0x55;
    tx_version.tx_buff[1] = version_len + 3;
    tx_version.tx_buff[2] = 6;
    memcpy(&tx_version.tx_buff[3], SOFTWARE_VERSION, version_len);
    tx_version.tx_buff[version_len + 3] =
        calculate_crc16(&tx_version.tx_buff[3], version_len) >> 8;
    tx_version.tx_buff[version_len + 4] =
        calculate_crc16(&tx_version.tx_buff[3], version_len);
    tx_version.tx_buff[version_len + 5] =
        calculate_buff_checksum(&tx_version.tx_buff[0], version_len + 5);
    drv_usart4_send_buffer(tx_version.tx_buff, version_len + 6);
}

void ota_file_request(uint32_t offset, uint8_t datalen) {
    tx_version.tx_buff[0] = 0x55;
    tx_version.tx_buff[1] = 8;
    tx_version.tx_buff[2] = 7;
    tx_version.tx_buff[3] = (uint8_t)(offset >> 24);
    tx_version.tx_buff[4] = (uint8_t)(offset >> 16);
    tx_version.tx_buff[5] = (uint8_t)(offset >> 8);
    tx_version.tx_buff[6] = (uint8_t)(offset);
    tx_version.tx_buff[7] = datalen;
    tx_version.tx_buff[8] = calculate_crc16(&tx_version.tx_buff[3], 5) >> 8;
    tx_version.tx_buff[9] = calculate_crc16(&tx_version.tx_buff[3], 5);
    tx_version.tx_buff[10] =
        calculate_buff_checksum(&tx_version.tx_buff[0], 10);
    drv_usart4_send_buffer(tx_version.tx_buff, 11);
}

void ota_init() {
    memset(&usart4, 0, sizeof(usart_rx_info_stu_t));
    fal_init();
    HAL_UART_Receive_IT(&huart4, &usart4.rx_data, 1);
}

void ota_recv(UART_HandleTypeDef *huart) {
    if (HAL_GetTick() >= usart4.timeout + 100) {
        usart4.rx_status = 0;
    }
    usart4.timeout = HAL_GetTick();

    SEGGER_RTT_printf(0, "rx_buff[%d] = %lx\r\n", usart4.rx_ptr,
                      usart4.rx_data);
    switch (usart4.rx_status) {
        case 0:
            if (usart4.rx_data == 0x55) {
                usart4.rx_ptr = 0;
                memset(usart4.rx_buff, 0, sizeof(usart4.rx_buff));
                usart4.rx_buff[usart4.rx_ptr++] = usart4.rx_data;
                usart4.rx_status++;
            }
            break;

        case 1:
            if (usart4.rx_data + 3 < 0xFF) {
                usart4.frame_len = usart4.rx_data + 3;
                usart4.rx_buff[usart4.rx_ptr++] = usart4.rx_data;
                usart4.rx_status++;
            } else {
                usart4.rx_status = 0;
            }
            break;

        case 2:
            usart4.rx_buff[usart4.rx_ptr++] = usart4.rx_data;
            if (usart4.rx_ptr == usart4.frame_len) {
                usart4.rx_status = 0;
                if ((uint8_t) calculate_buff_checksum(usart4.rx_buff,
                                                      usart4.frame_len - 1) ==
                    usart4.rx_data) {
                    usart4.frame_cmd = usart4.rx_buff[2];
                    usart4.frame_finish = 1;
                    SEGGER_RTT_printf(0, "finish\r\n");
                } else {
                    SEGGER_RTT_printf(0, "want %2x get %2x\r\n",
                                      calculate_buff_checksum(
                                          usart4.rx_buff, usart4.frame_len - 1),
                                      usart4.rx_data);
                }
            }
            break;
    }
    HAL_UART_Receive_IT(huart, &usart4.rx_data, 1);
}

void ota_msg_parse(usart_rx_info_stu_t *usart) {
    if (usart->frame_finish == 1) {
        usart->frame_finish = 0;
        uint16_t crc16 = (uint16_t)(usart->rx_buff[usart->frame_len - 3] << 8) +
                         usart->rx_buff[usart->frame_len - 2];
        if (calculate_crc16(&usart->rx_buff[3], usart->frame_len - 6) ==
            crc16) {
            switch (usart->frame_cmd) {
                case 6:
                    ota.filelen = (uint32_t)(usart->rx_buff[3] << 24) +
                                  (uint32_t)(usart->rx_buff[4] << 16) +
                                  (uint32_t)(usart->rx_buff[5] << 8) +
                                  usart->rx_buff[6];
                    memset(ota.md5str, 0, sizeof(ota.md5str));
                    memcpy(ota.md5str, &usart->rx_buff[7], 32);
                    ota.md5str[33] = '\0';
                    SEGGER_RTT_printf(0, "len %d\r\n", ota.filelen);
                    SEGGER_RTT_printf(0, "md5 %s\r\n", ota.md5str);
                    ota.package_req = 1;
                    ota.startime = HAL_GetTick();
                    ota.timeout = HAL_GetTick();
                    ota_start = 1;
                    break;

                case 7:
                    ota.package_req = 1;
                    ota.timeout = HAL_GetTick();
                    break;

                default:
                    break;
            }
            SEGGER_RTT_printf(0, "crc16 is ok!\n");
        } else {
            SEGGER_RTT_printf(0, "crc16 is error!\n");
        }
    }
}

void ota_server(void) {
    /*帧数据解析*/
    ota_msg_parse(&usart4);

    /*2s内没有接受到包数据，重新请求*/
    if ((ota_start == 1) && (HAL_GetTick() >= ota.timeout + 2000)) {
        ota.timeout = HAL_GetTick();
        SEGGER_RTT_printf(0, "package req again\r\n");
        ota_file_request(ota.fileoffest, ota.datalen);
    }

    /*3min内没有升级成功，则升级失败*/
    if ((ota_start == 1) && (HAL_GetTick() >= ota.startime + 180000)) {
        SEGGER_RTT_printf(0, "ota timeout fail\r\n");
        ota_start = 0;
        ota.datalen = 0;
        ota.fileoffest = 0;
    }

    /*接送到一包数据，开始处理*/
    if (ota.package_req == 1) {
        ota.package_req = 0;
        const struct fal_partition *partition = NULL;
        partition = fal_partition_find("main_mcu_temp");
        if (partition == NULL) {
            SEGGER_RTT_printf(0, "Find partition (%s) failed!\r\n",
                              "main_mcu_temp");
        } else {
            /*开始升级，擦除备份区*/
            if (usart4.frame_cmd == 0x06) {
                if (fal_partition_erase_all(partition) != -1) {
                    SEGGER_RTT_printf(0, "erase ok\r\n");
                } else {
                    SEGGER_RTT_printf(0, "erase fail\r\n");
                }
            }
            /*写入数据到备份区*/
            else if (usart4.frame_cmd == 0x07) {
                if (fal_partition_write(partition, ota.fileoffest,
                                        &usart4.rx_buff[3],
                                        ota.datalen) != -1) {
                    ota.fileoffest += 200;
                    SEGGER_RTT_printf(0, "write offest %d and len %d ok\r\n",
                                      ota.fileoffest, ota.datalen);
                    HAL_GPIO_TogglePin(STATE_LED_GPIO_Port, STATE_LED_Pin);
                } else {
                    SEGGER_RTT_printf(0, "write offest %d error\r\n",
                                      ota.fileoffest);
                }
                /*数据接送完毕，进行md5校验*/
                if (ota.fileoffest >= ota.filelen) {
                    char string_temp[3] = {0};
                    char cal_md5[33] = {0};
                    uint8_t decrypt[16] = {0};
                    MD5_CTX md5;
                    uint32_t i = 0;

                    MD5Init(&md5);
                    for (i = 0; i < ota.filelen / 200; i++) {
                        fal_partition_read(partition, i * 200,
                                           &usart4.rx_buff[3], 200);
                        MD5Update(&md5, &usart4.rx_buff[3], 200);
                    }
                    fal_partition_read(partition, i * 200, &usart4.rx_buff[3],
                                       ota.filelen - i * 200);
                    MD5Update(&md5, &usart4.rx_buff[3], ota.filelen - i * 200);
                    MD5Final(&md5, decrypt);

                    for (uint8_t i = 0; i < 16; i++) {
                        sprintf(string_temp, "%02x", decrypt[i]);
                        strcat(cal_md5, string_temp);
                    }

                    if (memcmp(cal_md5, ota.md5str, 32)) {
                        SEGGER_RTT_printf(0, "total md5 cal     : %s\r\n",
                                          cal_md5);
                        SEGGER_RTT_printf(0, "total md5 receive : %s\r\n",
                                          ota.md5str);
                        HAL_GPIO_WritePin(STATE_LED_GPIO_Port, STATE_LED_Pin,
                                          SET);
                        HAL_Delay(3000);
                    } else {
                        SEGGER_RTT_printf(0, "md5 check ok! \r\n");
                        SEGGER_RTT_printf(0,
                                          "----------update go----------\r\n");
                        __set_FAULTMASK(1);      // 关闭所有中断
                        HAL_NVIC_SystemReset();  // 复位
                    }
                    ota.fileoffest = 0;
                    ota_start = 0;
                    return;
                }
            }
            /*请求200字节的数据，当剩余字节不足200时，请求剩余字节数*/
            if (ota.filelen >= (ota.fileoffest + 200)) {
                ota.datalen = 200;
            } else {
                ota.datalen = ota.filelen - ota.fileoffest;
            }

            ota_file_request(ota.fileoffest, ota.datalen);
        }
    }
}