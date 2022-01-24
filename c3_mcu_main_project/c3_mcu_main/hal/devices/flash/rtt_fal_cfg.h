/*
 * File      : fal_cfg.h
 * This file is part of FAL (Flash Abstraction Layer) package
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32_onchip_flash;
extern const struct fal_flash_dev stm32_onchip_tsdb_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32_onchip_flash,                                             \
    &stm32_onchip_tsdb_flash,                                        \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG

#define FLASH_LEN                   (3076 * 1024)
#define OTA_BOOT_LEN                (128 * 1024)
#define OTA_APP_LEN                 (512 * 1024)
#define OTA_FILETEMP_LEN            (512 * 1024)
#define OTA_ROLLBACK_LEN            (512 * 1024)
#define OTA_PAR_LEN                 (384 * 1024)
#define OTA_LOG_LEN                 (1024 * 1024)

#define OTA_BOOT_ADDR               (0)
#define OTA_APP_ADDR                (OTA_BOOT_LEN)
#define OTA_FILETEMP_ADDR           (OTA_BOOT_LEN + OTA_APP_LEN)
#define OTA_ROLLBACK_ADDR           (OTA_BOOT_LEN + OTA_APP_LEN + OTA_FILETEMP_LEN)
#define OTA_PAR_ADDR                (OTA_BOOT_LEN + OTA_APP_LEN + OTA_FILETEMP_LEN + OTA_ROLLBACK_LEN)
#define OTA_LOG_ADDR                (OTA_BOOT_LEN + OTA_APP_LEN + OTA_FILETEMP_LEN + OTA_ROLLBACK_LEN + + OTA_PAR_LEN)
/* partition table */
#define FAL_PART_TABLE                                                                                           \
{                                                                                                                \
    {FAL_PART_MAGIC_WORD,"bl",               "stm32_onchip",        OTA_BOOT_ADDR,        OTA_BOOT_LEN,      0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_app",     "stm32_onchip",        OTA_APP_ADDR,         OTA_APP_LEN,       0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_temp",    "stm32_onchip",        OTA_FILETEMP_ADDR,    OTA_FILETEMP_LEN,  0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_rollback","stm32_onchip",        OTA_ROLLBACK_ADDR,    OTA_ROLLBACK_LEN,  0}, \
    {FAL_PART_MAGIC_WORD,"fdb_kvdb",         "stm32_onchip",        OTA_PAR_ADDR,         OTA_PAR_LEN,       0}, \
    {FAL_PART_MAGIC_WORD,"fdb_tsdb",         "stm32_onchip_tsdb",   OTA_LOG_ADDR,         OTA_LOG_LEN,       0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
