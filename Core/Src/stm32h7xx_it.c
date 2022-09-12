/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef heth;
extern LTDC_HandleTypeDef hltdc;
extern DMA_HandleTypeDef hdma_sai1_a;
extern SD_HandleTypeDef hsd1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
//void HardFault_Handler(void)
//{
//  /* USER CODE BEGIN HardFault_IRQn 0 */

//  /* USER CODE END HardFault_IRQn 0 */
//  while (1)
//  {
//    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
//    /* USER CODE END W1_HardFault_IRQn 0 */
//  }
//}

/**
  * @brief This function handles Memory management fault.
  */
//void MemManage_Handler(void)
//{
//  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

//  /* USER CODE END MemoryManagement_IRQn 0 */
//  while (1)
//  {
//    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
//    /* USER CODE END W1_MemoryManagement_IRQn 0 */
//  }
//}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
#include "MusicDecode.h"


void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */
  rt_interrupt_enter();
	
	if(DMA1_Stream0->CR&DMA_SxCR_CT){
		rt_mq_send_wait(mq_music , WRITE_TO_BUFF1 , strlen(WRITE_TO_BUFF1) + 1 , 2);	
	}else{
		rt_mq_send_wait(mq_music , WRITE_TO_BUFF0 , strlen(WRITE_TO_BUFF0) + 1 , 2);
	}
  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sai1_a);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */
  rt_interrupt_leave();
  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles Ethernet global interrupt.
  */
void ETH_IRQHandler(void)
{
  /* USER CODE BEGIN ETH_IRQn 0 */
   rt_interrupt_enter();
  /* USER CODE END ETH_IRQn 0 */
  HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */
   rt_interrupt_leave();
  /* USER CODE END ETH_IRQn 1 */
}

/**
  * @brief This function handles SDMMC1 global interrupt.
  */
void SDMMC1_IRQHandler(void)
{
  /* USER CODE BEGIN SDMMC1_IRQn 0 */
   rt_interrupt_enter();
  /* USER CODE END SDMMC1_IRQn 0 */
  HAL_SD_IRQHandler(&hsd1);
  /* USER CODE BEGIN SDMMC1_IRQn 1 */
  rt_interrupt_leave();
  /* USER CODE END SDMMC1_IRQn 1 */
}

/**
  * @brief This function handles LTDC global interrupt.
  */
void LTDC_IRQHandler(void)
{
  /* USER CODE BEGIN LTDC_IRQn 0 */
   rt_interrupt_enter();
  /* USER CODE END LTDC_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* USER CODE BEGIN LTDC_IRQn 1 */
    rt_interrupt_leave();
  /* USER CODE END LTDC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

#include "lvgl.h"

extern void lv_disp_flush_ready(lv_disp_drv_t * disp_drv);
extern lv_disp_drv_t MyDisp_drv;

void DMA2D_IRQHandler(void){
	
	  rt_interrupt_enter();
	
		if(DMA2D->ISR&0x02){
			 lv_disp_flush_ready(&MyDisp_drv);
			 DMA2D->IFCR|=0x02;
		}

    rt_interrupt_leave();		
}


//void SysTick_Handler(void)
//{
//	  HAL_IncTick();
//	
//	  lv_tick_inc(1);
//}


/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
