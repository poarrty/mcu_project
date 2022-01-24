#include <stdint.h>
#define USART_BUFF_MAX_SIZE 260

typedef struct {
    uint8_t tx_buff[USART_BUFF_MAX_SIZE];
} usart_tx_info_stu_t;

typedef struct {
    uint8_t rx_data;
    uint8_t rx_status;
    uint8_t rx_ptr;
    uint32_t timeout;
    //帧长度
    uint8_t frame_len;
    //帧命令
    uint8_t frame_cmd;
    //帧完成标识
    uint8_t frame_finish;
    //接收数组
    uint8_t rx_buff[USART_BUFF_MAX_SIZE];
} usart_rx_info_stu_t;

typedef struct {
    uint32_t startime;
    uint32_t timeout;
    uint32_t filelen;
    char md5str[33];

    uint8_t datalen;
    uint8_t package_req;
    uint32_t fileoffest;
} ota_stu_t;

uint8_t ota_start;
void ota_server(void);
void ota_send_version(uint16_t frq);
void ota_recv(UART_HandleTypeDef *huart);
void ota_init();