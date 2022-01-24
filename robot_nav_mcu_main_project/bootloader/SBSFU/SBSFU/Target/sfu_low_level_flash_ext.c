/**
  ******************************************************************************
  * @file    sfu_low_level_flash_ext.c
  * @author  MCD Application Team
  * @brief   SFU Flash Low Level Interface module
  *          This file provides set of firmware functions to manage SFU external
  *          flash low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level_flash_ext.h"
#include "sfu_low_level_security.h"

#include <sfud.h>
#include <flashdb.h>
#include <time.h>

//#define FLASH_EXT_LOG

void Spi_Flash_Init(void);
void ota_read_file_temp(uint32_t addr, uint8_t *buffer, uint32_t count);

static void lock(fdb_db_t db);
static void unlock(fdb_db_t db);

uint8_t y_update_flag = 0;

/* KVDB object */
struct fdb_kvdb kvdb = { 0 };
/* TSDB object */
struct fdb_tsdb tsdb = { 0 };

/* Private defines -----------------------------------------------------------*/
#define BLOC_64_KBYTES 0x10000U

/* Functions Definition ------------------------------------------------------*/

/* No external flash available on this product
   ==> return SFU_ERROR except for SFU_LL_FLASH_EXT_Init and SFU_LL_FLASH_EXT_Config_Exe which are called
       systematically during startup phase */

SFU_ErrorStatus SFU_LL_FLASH_EXT_Init(void)
{
  Spi_Flash_Init();
  return SFU_SUCCESS;
}

SFU_ErrorStatus SFU_LL_FLASH_EXT_Erase_Size(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t *pStart, uint32_t Length)
{
  /*
  UNUSED(pFlashStatus);
  UNUSED(pStart);
  UNUSED(Length);
  return SFU_ERROR;
  */

#if defined FLASH_EXT_LOG
  printf("\r\nFLASH_EXT_ERASE： 0X%X（%d)\r\n", (uint32_t)pStart, Length);
#endif

  const struct fal_partition *partition = NULL;

  partition = fal_partition_find("main_mcu_temp");

  if((uint32_t)pStart < 0x080E0000)
  {
    partition = fal_partition_find("main_mcu_temp");
    pStart = (uint32_t)pStart-EXTERNAL_FLASH_ADDRESS;
  }
  else
  {
    partition = fal_partition_find("boot_swap");
    pStart = (uint32_t)pStart-0x080E0000;
  }

  if (partition == NULL)
  {
    printf("\r\n[I/FAL] Find partition (%s) failed!\r\n", "main_mcu_temp");
    return SFU_ERROR;
  }

  fal_partition_erase(partition, (uint32_t)pStart, Length);

  *pFlashStatus = SFU_FLASH_SUCCESS;

  return SFU_SUCCESS;

}
uint8_t buff_header[320] = {0};
SFU_ErrorStatus SFU_LL_FLASH_EXT_Write(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t  *pDestination,
                                       const uint8_t *pSource, uint32_t Length)
{
  /*
  UNUSED(pFlashStatus);
  UNUSED(pDestination);
  UNUSED(pSource);
  UNUSED(Length);
  return SFU_ERROR;
  */

#if defined FLASH_EXT_LOG
  printf("\r\nFLASH_EXT_W： 0X%X（%d)\r\n", (uint32_t)pDestination, Length);
#endif

  const struct fal_partition *partition = NULL;

  if((uint32_t)pDestination < 0x080E0000)
  {
    partition = fal_partition_find("main_mcu_temp");
    pDestination = (uint32_t)pDestination-EXTERNAL_FLASH_ADDRESS;
  }
  else
  {
    partition = fal_partition_find("boot_swap");
    pDestination = (uint32_t)pDestination-0x080E0000;
  }

  
  if (partition == NULL)
  {
    printf("\r\n[I/FAL] Find partition (%s) failed!\r\n", "main_mcu_temp");
    return SFU_ERROR;
  }

  if(fal_partition_write(partition, (uint32_t)pDestination, pSource, Length) < 0)
  {
	  printf("\r\n SPI flash write error!\r\n");
	  return SFU_ERROR;
  }

 *pFlashStatus = SFU_FLASH_SUCCESS;

   if((uint32_t)pDestination == 0x08080000)
  {
	  //memcpy(buff_header, pSource, 320);
	  fal_partition_read(partition, (uint32_t)pDestination, buff_header, 320);
  }
  
  return SFU_SUCCESS;

}

SFU_ErrorStatus SFU_LL_FLASH_EXT_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length)
{
  /*
  UNUSED(pDestination);
  UNUSED(pSource);
  UNUSED(Length);
  return SFU_ERROR;
  */

#if defined FLASH_EXT_LOG
  printf("\r\nFLASH_EXT_R： 0X%X（%d)\r\n", (uint32_t)pSource, Length);
#endif

  const struct fal_partition *partition = NULL;

  if((uint32_t)pSource < 0x080E0000)
  {
    partition = fal_partition_find("main_mcu_temp");
    pSource = (uint32_t)pSource-EXTERNAL_FLASH_ADDRESS;
  }
  else
  {
    partition = fal_partition_find("boot_swap");
    pSource = (uint32_t)pSource-0x080E0000;
  }

  if (partition == NULL)
  {
    printf("\r\n[I/FAL] Find partition (%s) failed!\r\n", "main_mcu_temp");
    return SFU_ERROR;
  }

  fal_partition_read(partition, (uint32_t)pSource, pDestination, Length);

  /*
  if((uint32_t)pSource == 0x08080000 && Length == 320)
  {
	  printf("\r\nPrint dwslot header: \r\n");

	  uint32_t len  = 0;
	  while(len < Length)
	  {
		  printf("0X%02X ", pDestination[len]);
		  len++;
	  }
	  
	  printf("\r\nPrint dwslot header rece: \r\n");
	  len = 0;
	  while(len < 320)
	  {
		  printf("0X%02X ", buff_header[len]);
		  len++;
	  }
  }
  */

  return SFU_SUCCESS;

}

SFU_ErrorStatus SFU_LL_FLASH_EXT_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length)
{
  /*
  UNUSED(pFlash);
  UNUSED(Pattern1);
  UNUSED(Pattern2);
  UNUSED(Length);
  return SFU_ERROR;
  */
  return SFU_SUCCESS;

}

SFU_ErrorStatus SFU_LL_FLASH_EXT_Config_Exe(uint32_t SlotNumber)
{
  UNUSED(SlotNumber);
  return SFU_SUCCESS;
}

void Spi_Flash_Init(void)
{
	if (sfud_init() != SFUD_SUCCESS)
	{
		printf("\r\n\r\n[SFUD] SFUD init fail.\r\n");
	}
	else
	{
		printf("\r\n\r\n[SFUD] SFUD init ok.\r\n");
	}

	struct fdb_default_kv default_kv;

	default_kv.kvs = NULL;

	default_kv.num = 0;

	/* set the lock and unlock function if you want */
	fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, lock);

	fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, unlock);

	/* Key-Value database initialization
	*
	*       &kvdb: database object
	*       "env": database name
	* "fdb_kvdb": The flash partition name base on FAL. Please make sure it's in FAL partition table.
	*              Please change to YOUR partition name.
	* &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
	*        NULL: The user data if you need, now is empty.
	*/
	if (fdb_kvdb_init(&kvdb, "env", "fdb_kvdb", &default_kv, NULL) != FDB_NO_ERR)
	{
		printf("[FlashDB] fdb_kvdb init fail\r\n");
	}
	else
	{
		printf("[FlashDB] fdb_kvdb init ok\r\n");
	}

  struct fdb_blob blob;
  
  uint8_t len;

  len = fdb_kv_get_blob(&kvdb, "y_update_flag", fdb_blob_make(&blob, (uint8_t *)&y_update_flag, sizeof(y_update_flag)));

  if(len == 0)
  {
    printf("Ymodem update flag not found, create it.\r\n");
    fdb_kv_set_blob(&kvdb, "y_update_flag", fdb_blob_make(&blob, (uint8_t *)&y_update_flag, sizeof(y_update_flag)));
  }

  printf("Ymodem update flag: %d\r\n", y_update_flag);

}

void ota_read_file_temp(uint32_t addr, uint8_t *buffer, uint32_t count)
{
	const struct fal_partition *partition = NULL;

	partition = fal_partition_find("main_mcu_temp");

	if (partition == NULL)
	{
		printf("\r\nFind partition (%s) failed!\r\n", "main_mcu_temp");
		return;
	}

	fal_partition_read(partition, addr, buffer, count);
}

static void lock(fdb_db_t db)
{
	__disable_irq();
}

static void unlock(fdb_db_t db)
{
	__enable_irq();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
