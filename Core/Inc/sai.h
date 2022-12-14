/**
  ******************************************************************************
  * @file    sai.h
  * @brief   This file contains all the function prototypes for
  *          the sai.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SAI_H__
#define __SAI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SAI_HandleTypeDef hsai_BlockA1;
extern DMA_HandleTypeDef hdma_sai1_a;
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SAI1_Init(void);

/* USER CODE BEGIN Prototypes */
void SAI1_Config(uint32_t mode,uint32_t Frequncy,uint8_t Datasize,uint8_t slots);
void ASI_DMA_DoubleBufferTransferStart(uint32_t Buf0,uint32_t Buf1,uint32_t size,uint32_t num);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SAI_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
