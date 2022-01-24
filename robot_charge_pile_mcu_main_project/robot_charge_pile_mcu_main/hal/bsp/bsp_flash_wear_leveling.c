#include "bsp_flash_wear_leveling.h"
#include "bsp_flash.h"
#include "stm32f1xx_hal_crc.h"
#include "crc16.h"
#include "log.h"

extern CRC_HandleTypeDef hcrc;

EEPromType EEData;
EEPromType EEData_Backup;
uint32_t EEData_Address = 0;

#define EE_ReadTbl Flash_ReadTbl
#define EE_WriteTbl Flash_WriteTbl
#define EE_EraseSector Flash_EraseSector

void Init_SystemSetting(void) {
    memset(&EEData.System, 0, sizeof(EEData.System));
    EEData.System.Dummy = 0x1234;
}

void Init_UserSetting(void) {
    memset(&EEData.User, 0, sizeof(EEData.User));
    EEData.User.Dummy = 0x5678;
}

void CheckUserSettingRange(void) {}

uint32_t PrepareNextVaildAddress(EEPromType *Buffer, uint32_t CurrentAddress,
                                 uint16_t Size, uint32_t StartAddress,
                                 uint32_t EndAddress) {
    uint32_t i;
    uint32_t NextEndAddress;
    uint32_t Sector_Current, Sector_Next;
    uint32_t NextAddress = CurrentAddress;
    uint8_t *BufferData = (uint8_t *) Buffer;
    Sector_Current =
        (CurrentAddress + Size - 1 - FLASH_BASE_ADDR) / NVRamAddress_SectorSize;

    do {
        NextAddress = NextAddress + Size;
        NextEndAddress = NextAddress + Size - 1;

        if (NextEndAddress > EndAddress) {
            NextAddress = StartAddress;
            NextEndAddress = NextAddress + Size - 1;
        }

        Sector_Next =
            (NextEndAddress - FLASH_BASE_ADDR) / NVRamAddress_SectorSize;

        if (Sector_Next != Sector_Current) {
            NextAddress =
                Sector_Next * NVRamAddress_SectorSize + FLASH_BASE_ADDR;
            EE_EraseSector(NextAddress);

            if (NextAddress < StartAddress) {
                NextAddress = StartAddress;
            }
        }

        EE_ReadTbl(NextAddress, (uint8_t *) BufferData, Size);

        for (i = 0; i < Size; i++) {
            if (BufferData[i] != NVRamAddress_Empty) {
                break;
            }
        }

        if (i >= Size) {
            return NextAddress;
        }
    } while (NextAddress == CurrentAddress);

    EE_EraseSector(StartAddress);
    return StartAddress;
}

uint32_t SaveEESetting(EEPromType *Buffer, EEPromType *Buffer_Backup,
                       uint32_t CurrentAddress, uint16_t Size,
                       uint32_t StartAddress, uint32_t EndAddress) {
    if (memcmp(Buffer_Backup, Buffer, Size) != 0) {
        Buffer->Index++;
        Buffer->Inv_Index = ~Buffer->Index;
        EE_WriteTbl(CurrentAddress, (uint8_t *) Buffer, Size);
        CurrentAddress = PrepareNextVaildAddress(
            Buffer_Backup, CurrentAddress, Size, StartAddress, EndAddress);
        LOG_INFO("EE SAVE NextVaildAddress = 0x%lx\n", CurrentAddress);
        memcpy(Buffer_Backup, Buffer, Size);
    }

    return CurrentAddress;
}

void SaveUserSetting(void) {
    uint32_t CRCSum;
    CRCSum = HAL_CRC_Calculate(&hcrc, (uint32_t *) &EEData.User,
                               (UserSettingSize / 4) - 1);
    // CRCSum = calculate_crc16((uint8_t *) &EEData.User, (UserSettingSize / 4)
    // - 1);

    if (EEData.User.Crc != CRCSum) {
        EEData.User.Crc = CRCSum;
        EEData_Address = SaveEESetting(&EEData, &EEData_Backup, EEData_Address,
                                       EEDataSize, (uint32_t) NVRamAddress_Base,
                                       (uint32_t) NVRamAddress_End);
    } else {
        LOG_INFO("UserSetting same CRC,Save Nothing\n");
    }
}

void SaveSystemSetting(void) {
    uint32_t CRCSum;
    CRCSum = HAL_CRC_Calculate(&hcrc, (uint32_t *) &EEData.System,
                               (SystemSettingSize / 4) - 1);
    // CRCSum = calculate_crc16((uint8_t *) &EEData.System, (SystemSettingSize /
    // 4) - 1);

    if (EEData.System.Crc != CRCSum) {
        EEData.System.Crc = CRCSum;
        EEData_Address =
            SaveEESetting(&EEData, &EEData_Backup, EEData_Address, EEDataSize,
                          NVRamAddress_Base, NVRamAddress_End);
        LOG_INFO("SaveSystemSetting is Finish\n");
        LOG_INFO("PrepareNextVaildAddress = 0x%lx\n", EEData_Address);
    }
}

void Init_EEPRomSetting(void) {
    EEData_Backup.Index = 0x00000000;
    EEData.Index = 0xFFFFFFFF;
    Init_SystemSetting();
    EEData.System.Crc = HAL_CRC_Calculate(&hcrc, (uint32_t *) &EEData.System,
                                          (SystemSettingSize / 4) - 1);
    // EEData.System.Crc = calculate_crc16((uint8_t *) &EEData.System,
    // (SystemSettingSize / 4) - 1);

    Init_UserSetting();
    CheckUserSettingRange();
    EEData.User.Crc = HAL_CRC_Calculate(&hcrc, (uint32_t *) &EEData.User,
                                        (UserSettingSize / 4) - 1);
    // EEData.User.Crc = calculate_crc16((uint8_t *) &EEData.User,
    // (UserSettingSize / 4) - 1);

    EEData_Address = NVRamAddress_Base;

    if (HAL_OK == EE_EraseSector(EEData_Address)) {
        LOG_INFO("EraseSector OK");
        EEData_Address = SaveEESetting(&EEData, &EEData_Backup, EEData_Address,
                                       EEDataSize, (uint32_t) NVRamAddress_Base,
                                       (uint32_t) NVRamAddress_End);
        LOG_INFO("Init_EEPRomSetting SaveEEData OK\n");
    }
}

uint32_t GetNextAddress(uint32_t CurrentAddress, uint32_t Size) {
    uint32_t Sector_Current, Sector_Next;
    Sector_Current =
        (CurrentAddress + Size - 1 - FLASH_BASE_ADDR) / NVRamAddress_SectorSize;
    Sector_Next = (CurrentAddress + Size + Size - 1 - FLASH_BASE_ADDR) /
                  NVRamAddress_SectorSize;

    if (Sector_Next == Sector_Current) {
        return CurrentAddress + Size;
    } else {
        return Sector_Next * NVRamAddress_SectorSize + FLASH_BASE_ADDR;
    }
}

uint32_t GetFirstVaildMultiCopyBlock(EEPromType *Buffer, uint32_t Size,
                                     uint32_t StartAddress,
                                     uint32_t EndAddress) {
    uint32_t Index_Address = StartAddress;

    while (Index_Address + Size - 1 <= EndAddress) {
        EE_ReadTbl(Index_Address, (uint8_t *) (Buffer), Size);

        if (Buffer->Index == ~Buffer->Inv_Index) {
            LOG_INFO("First time GetNextVaildAddress = 0x%lx\n", Index_Address);
            return Index_Address;
        }

        Index_Address = GetNextAddress(Index_Address, Size);
    }

    return 0xFFFFFFFF;
}

void Read_Settings(void) {
    uint32_t Last_Vaild_Index_Value;
    uint32_t Last_Vaild_Index_Address;

    LOG_INFO("Sizeof(SystemSettingSize) = %d\n", SystemSettingSize);
    LOG_INFO("Sizeof(UserSettingSize) = %d\n", UserSettingSize);
    LOG_INFO("Sizeof(EEData) = %d\n", EEDataSize);

    Last_Vaild_Index_Address = GetFirstVaildMultiCopyBlock(
        &EEData, EEDataSize, NVRamAddress_Base, NVRamAddress_End);

    if (Last_Vaild_Index_Address == 0xFFFFFFFF) {
        LOG_INFO("Init_EEPRomSetting\n");
        Init_EEPRomSetting();
    } else {
        Last_Vaild_Index_Value = EEData.Index;
        EEData_Address = GetNextAddress(Last_Vaild_Index_Address, EEDataSize);

        while (EEData_Address + EEDataSize - 1 <= NVRamAddress_End) {
            EE_ReadTbl(EEData_Address, (uint8_t *) (&EEData), EEDataSize);

            if (EEData.Index == ~EEData.Inv_Index) {
                if (EEData.Index != (Last_Vaild_Index_Value + 1)) {
                    break;
                } else {
                    Last_Vaild_Index_Value = EEData.Index;
                    Last_Vaild_Index_Address = EEData_Address;
                }
            }

            EEData_Address = GetNextAddress(EEData_Address, EEDataSize);
        }

        // LastOne Found

        EEData_Address = Last_Vaild_Index_Address;
        EE_ReadTbl(EEData_Address, (uint8_t *) (&EEData), EEDataSize);
        LOG_INFO("Last Vaild EEData_Address = 0x%lx\n", EEData_Address);
        EEData_Address =
            PrepareNextVaildAddress(&EEData_Backup, EEData_Address, EEDataSize,
                                    NVRamAddress_Base, NVRamAddress_End);
        LOG_INFO("current Vaild EEData_Address = 0x%lx\n", EEData_Address);
        memcpy(&EEData_Backup, &EEData, EEDataSize);

        // if (EEData.System.Crc != calculate_crc16((uint8_t *) &EEData.System,
        // (SystemSettingSize / 4) - 1))
        if (EEData.System.Crc !=
            HAL_CRC_Calculate(&hcrc, (uint32_t *) &EEData.System,
                              (SystemSettingSize / 4) - 1)) {
            LOG_INFO("Initial SystemSetting\n");
            Init_SystemSetting();
            SaveSystemSetting();
        }

        // if (EEData.User.Crc != calculate_crc16((uint8_t *) &EEData.User,
        // (UserSettingSize / 4) - 1))
        if (EEData.User.Crc != HAL_CRC_Calculate(&hcrc,
                                                 (uint32_t *) &EEData.User,
                                                 (UserSettingSize / 4) - 1)) {
            LOG_INFO("Initial UserSetting\n");
            Init_UserSetting();
            SaveUserSetting();
        }
    }
}

void bsp_read_simeeprom(void) {
    Read_Settings();
}

void bsp_write_simeeprom(void) {
    SaveSystemSetting();
    SaveUserSetting();
}