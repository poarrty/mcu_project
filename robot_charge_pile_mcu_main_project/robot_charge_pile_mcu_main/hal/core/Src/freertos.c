/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_charge */
osThreadId_t task_chargeHandle;
uint32_t task_chargeBuffer[256];
osStaticThreadDef_t task_chargeControlBlock;
const osThreadAttr_t task_charge_attributes = {
    .name = "task_charge",
    .cb_mem = &task_chargeControlBlock,
    .cb_size = sizeof(task_chargeControlBlock),
    .stack_mem = &task_chargeBuffer[0],
    .stack_size = sizeof(task_chargeBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_led */
osThreadId_t task_ledHandle;
uint32_t task_ledBuffer[256];
osStaticThreadDef_t task_ledControlBlock;
const osThreadAttr_t task_led_attributes = {
    .name = "task_led",
    .cb_mem = &task_ledControlBlock,
    .cb_size = sizeof(task_ledControlBlock),
    .stack_mem = &task_ledBuffer[0],
    .stack_size = sizeof(task_ledBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_digital_tu */
osThreadId_t task_digital_tuHandle;
uint32_t task_digital_tuBuffer[256];
osStaticThreadDef_t task_digital_tuControlBlock;
const osThreadAttr_t task_digital_tu_attributes = {
    .name = "task_digital_tu",
    .cb_mem = &task_digital_tuControlBlock,
    .cb_size = sizeof(task_digital_tuControlBlock),
    .stack_mem = &task_digital_tuBuffer[0],
    .stack_size = sizeof(task_digital_tuBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_battery */
osThreadId_t task_batteryHandle;
uint32_t task_batteryBuffer[256];
osStaticThreadDef_t task_batteryControlBlock;
const osThreadAttr_t task_battery_attributes = {
    .name = "task_battery",
    .cb_mem = &task_batteryControlBlock,
    .cb_size = sizeof(task_batteryControlBlock),
    .stack_mem = &task_batteryBuffer[0],
    .stack_size = sizeof(task_batteryBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_temp */
osThreadId_t task_tempHandle;
uint32_t task_tempBuffer[256];
osStaticThreadDef_t task_tempControlBlock;
const osThreadAttr_t task_temp_attributes = {
    .name = "task_temp",
    .cb_mem = &task_tempControlBlock,
    .cb_size = sizeof(task_tempControlBlock),
    .stack_mem = &task_tempBuffer[0],
    .stack_size = sizeof(task_tempBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_shell */
osThreadId_t task_shellHandle;
uint32_t task_shellBuffer[512];
osStaticThreadDef_t task_shellControlBlock;
const osThreadAttr_t task_shell_attributes = {
    .name = "task_shell",
    .cb_mem = &task_shellControlBlock,
    .cb_size = sizeof(task_shellControlBlock),
    .stack_mem = &task_shellBuffer[0],
    .stack_size = sizeof(task_shellBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_ir_38k */
osThreadId_t task_ir_38kHandle;
uint32_t task_ir_38kBuffer[256];
osStaticThreadDef_t task_ir_38kControlBlock;
const osThreadAttr_t task_ir_38k_attributes = {
    .name = "task_ir_38k",
    .cb_mem = &task_ir_38kControlBlock,
    .cb_size = sizeof(task_ir_38kControlBlock),
    .stack_mem = &task_ir_38kBuffer[0],
    .stack_size = sizeof(task_ir_38kBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_key */
osThreadId_t task_keyHandle;
uint32_t task_keyBuffer[256];
osStaticThreadDef_t task_keyControlBlock;
const osThreadAttr_t task_key_attributes = {
    .name = "task_key",
    .cb_mem = &task_keyControlBlock,
    .cb_size = sizeof(task_keyControlBlock),
    .stack_mem = &task_keyBuffer[0],
    .stack_size = sizeof(task_keyBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_workstation_clean */
osThreadId_t task_workstation_cleanHandle;
uint32_t task_workstation_cleanBuffer[256];
osStaticThreadDef_t task_workstation_cleanControlBlock;
const osThreadAttr_t task_workstation_clean_attributes = {
    .name = "task_workstation_clean",
    .cb_mem = &task_workstation_cleanControlBlock,
    .cb_size = sizeof(task_workstation_cleanControlBlock),
    .stack_mem = &task_workstation_cleanBuffer[0],
    .stack_size = sizeof(task_workstation_cleanBuffer),
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_test_mode */
osThreadId_t task_test_modeHandle;
uint32_t task_test_modeBuffer[256];
osStaticThreadDef_t task_test_modeControlBlock;
const osThreadAttr_t task_test_mode_attributes = {
    .name = "task_test_mode",
    .cb_mem = &task_test_modeControlBlock,
    .cb_size = sizeof(task_test_modeControlBlock),
    .stack_mem = &task_test_modeBuffer[0],
    .stack_size = sizeof(task_test_modeBuffer),
    .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void task_charge_run(void *argument);
void task_led_run(void *argument);
void task_digital_tube_run(void *argument);
void task_battery_run(void *argument);
void task_temp_run(void *argument);
void task_shell_run(void *argument);
void task_ir_38k_run(void *argument);
void task_key_run(void *argument);
void task_workstation_clean_run(void *argument);
void task_test_mode_run(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void) {}

__weak unsigned long getRunTimeCounterValue(void) {
    return 0;
}
/* USER CODE END 1 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle =
        osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of task_charge */
    task_chargeHandle =
        osThreadNew(task_charge_run, NULL, &task_charge_attributes);

    /* creation of task_led */
    task_ledHandle = osThreadNew(task_led_run, NULL, &task_led_attributes);

    /* creation of task_digital_tu */
    task_digital_tuHandle =
        osThreadNew(task_digital_tube_run, NULL, &task_digital_tu_attributes);

    /* creation of task_battery */
    task_batteryHandle =
        osThreadNew(task_battery_run, NULL, &task_battery_attributes);

    /* creation of task_temp */
    task_tempHandle = osThreadNew(task_temp_run, NULL, &task_temp_attributes);

    /* creation of task_shell */
    task_shellHandle =
        osThreadNew(task_shell_run, NULL, &task_shell_attributes);

    /* creation of task_ir_38k */
    task_ir_38kHandle =
        osThreadNew(task_ir_38k_run, NULL, &task_ir_38k_attributes);

    /* creation of task_key */
    task_keyHandle = osThreadNew(task_key_run, NULL, &task_key_attributes);

    /* creation of task_workstation_clean */
    task_workstation_cleanHandle = osThreadNew(
        task_workstation_clean_run, NULL, &task_workstation_clean_attributes);

    /* creation of task_test_mode */
    task_test_modeHandle =
        osThreadNew(task_test_mode_run, NULL, &task_test_mode_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument) {
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_task_charge_run */
/**
 * @brief Function implementing the task_charge thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_charge_run */
__weak void task_charge_run(void *argument) {
    /* USER CODE BEGIN task_charge_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_charge_run */
}

/* USER CODE BEGIN Header_task_led_run */
/**
 * @brief Function implementing the task_led thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_led_run */
__weak void task_led_run(void *argument) {
    /* USER CODE BEGIN task_led_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_led_run */
}

/* USER CODE BEGIN Header_task_digital_tube_run */
/**
 * @brief Function implementing the task_digital_tu thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_digital_tube_run */
__weak void task_digital_tube_run(void *argument) {
    /* USER CODE BEGIN task_digital_tube_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_digital_tube_run */
}

/* USER CODE BEGIN Header_task_battery_run */
/**
 * @brief Function implementing the task_battery thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_battery_run */
__weak void task_battery_run(void *argument) {
    /* USER CODE BEGIN task_battery_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_battery_run */
}

/* USER CODE BEGIN Header_task_temp_run */
/**
 * @brief Function implementing the task_temp thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_temp_run */
__weak void task_temp_run(void *argument) {
    /* USER CODE BEGIN task_temp_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_temp_run */
}

/* USER CODE BEGIN Header_task_shell_run */
/**
 * @brief Function implementing the task_shell thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_shell_run */
__weak void task_shell_run(void *argument) {
    /* USER CODE BEGIN task_shell_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_shell_run */
}

/* USER CODE BEGIN Header_task_ir_38k_run */
/**
 * @brief Function implementing the task_ir_38k thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_ir_38k_run */
__weak void task_ir_38k_run(void *argument) {
    /* USER CODE BEGIN task_ir_38k_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_ir_38k_run */
}

/* USER CODE BEGIN Header_task_key_run */
/**
 * @brief Function implementing the task_key thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_key_run */
__weak void task_key_run(void *argument) {
    /* USER CODE BEGIN task_key_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_key_run */
}

/* USER CODE BEGIN Header_task_workstation_clean_run */
/**
 * @brief Function implementing the task_workstation_clean thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_workstation_clean_run */
__weak void task_workstation_clean_run(void *argument) {
    /* USER CODE BEGIN task_workstation_clean_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_workstation_clean_run */
}

/* USER CODE BEGIN Header_task_test_mode_run */
/**
 * @brief Function implementing the task_test_mode thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_test_mode_run */
__weak void task_test_mode_run(void *argument) {
    /* USER CODE BEGIN task_test_mode_run */
    /* Infinite loop */
    for (;;) { osDelay(1); }
    /* USER CODE END task_test_mode_run */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
