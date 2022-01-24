/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: log.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 16:19:28
 * @Description: LOG接口定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include "common_def.h"
#include <stdio.h>
#include <string.h>

#if __LOG_EN__

#define COLOR_END "\033[0m"
#define INFO_COLOR "\033[36m"
#define DEBUG_COLOR "\033[35m"
#define DEBUG_ARRAY_COLOR "\033[35m"
#define WARNING_COLOR "\033[33m"
#define ERROR_COLOR "\033[31m"

#define filename(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x

#define LOG_INFO(format, ...)                                         \
    do {                                                              \
        if (log_level >= LOG_LEVEL_INFO) {                            \
            printf(INFO_COLOR "[INFO]:[FILE]:%s, [LINE]:%04d:" format \
                              "\r\n" COLOR_END,                       \
                   filename(__FILE__), __LINE__, ##__VA_ARGS__);      \
        }                                                             \
    } while (0)

#define LOG_DEBUG(format, ...)                                          \
    do {                                                                \
        if (log_level >= LOG_LEVEL_DEBUG) {                             \
            printf(DEBUG_COLOR "[DEBUG]:[FILE]:%s, [LINE]:%04d:" format \
                               "\r\n" COLOR_END,                        \
                   filename(__FILE__), __LINE__, ##__VA_ARGS__);        \
        }                                                               \
    } while (0)

#define LOG_DEBUG_ARRAY(array, num)                                       \
    do {                                                                  \
        if (log_level >= LOG_LEVEL_DEBUG_ARRAY) {                         \
            uint32_t i;                                                   \
            uint8_t *a = array;                                           \
            printf(DEBUG_ARRAY_COLOR                                      \
                   "[DEBUG_ARRAY]:[FILE]:%s, [LINE]:%04d:\r\n" COLOR_END, \
                   filename(__FILE__), __LINE__);                         \
            for (i = 0; i < num; i++) {                                   \
                printf("%#X   ", a[i]);                                   \
                if ((i + 1) % 10 == 0) {                                  \
                    printf("\r\n");                                       \
                }                                                         \
            }                                                             \
            printf("\r\n");                                               \
        }                                                                 \
    } while (0)

#define LOG_WARNING(format, ...)                                            \
    do {                                                                    \
        if (log_level >= LOG_LEVEL_WARNING) {                               \
            printf(WARNING_COLOR "[WARNING]:[FILE]:%s, [LINE]:%04d:" format \
                                 "\r\n" COLOR_END,                          \
                   filename(__FILE__), __LINE__, ##__VA_ARGS__);            \
        }                                                                   \
    } while (0)

#define LOG_ERROR(format, ...)                                          \
    do {                                                                \
        if (log_level >= LOG_LEVEL_ERROR) {                             \
            printf(ERROR_COLOR "[ERROR]:[FILE]:%s, [LINE]:%04d:" format \
                               "\r\n" COLOR_END,                        \
                   filename(__FILE__), __LINE__, ##__VA_ARGS__);        \
        }                                                               \
    } while (0)

#define LOG_CUSTOM(format, ...) printf(format, ##__VA_ARGS__);

#else

#define LOG_INFO(format, ...) \
    do {                      \
    } while (0)
#define LOG_DEBUG(format, ...) \
    do {                       \
    } while (0)
#define LOG_DEBUG_ARRAY(array, num) \
    do {                            \
    } while (0)
#define LOG_WARNING(format, ...) \
    do {                         \
    } while (0)
#define LOG_ERROR(format, ...) \
    do {                       \
    } while (0)
#define LOG_CUSTOM(format, ...) \
    do {                        \
    } while (0)

#endif

#define Error_Check_Callbake(status)            \
    do {                                        \
        if (status != 0) {                      \
            LOG_ERROR("Return val:%d", status); \
        }                                       \
    } while (0);

#endif
