#ifndef __BSP_FLASH_WEAR_LEVELING_H__
#define __BSP_FLASH_WEAR_LEVELING_H__

#include <stdint.h>
#include <string.h>
#include "fal_info_bind.h"
#include "fal_ota.h"

typedef struct {
    uint32_t Dummy;
    uint32_t Crc;
} SystemSettingType;

typedef struct {
    uint32_t Dummy;
    uint32_t Crc;
} UserSettingType;

typedef struct {
    uint32_t Index;
    SystemSettingType System;
    UserSettingType User;
    uint32_t Inv_Index;
} EEPromType;

#define FLASH_BASE_ADDR 0x08000000
#define FLASH_PAGE_SIZE 0x800U
#define FLASH_MAX_PAGE 192

#define EEPROM_ADDR_1 (FLASH_BASE_ADDR + (FLASH_MAX_PAGE - 1) * FLASH_PAGE_SIZE)
#define EEPROM_ADDR_2 (FLASH_BASE_ADDR + (FLASH_MAX_PAGE - 2) * FLASH_PAGE_SIZE)

#define EEDataSize sizeof(EEPromType)
#define SystemSettingSize sizeof(SystemSettingType)
#define UserSettingSize sizeof(UserSettingType)

#define NVRamAddress_SectorSize (FLASH_PAGE_SIZE)
#define NVRamAddress_Empty (0xff)
#define NVRamAddress_Size (FLASH_PAGE_SIZE * 2)  // 4k
#define NVRamAddress_Base (EEPROM_ADDR_2)
#define NVRamAddress_End (NVRamAddress_Base + NVRamAddress_Size - 1)

extern EEPromType EEData;
extern EEPromType EEData_Backup;

extern void bsp_read_simeeprom(void);
extern void bsp_write_simeeprom(void);

#endif
