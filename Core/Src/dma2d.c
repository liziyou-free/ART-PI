/**
  ******************************************************************************
  * @file    dma2d.c
  * @brief   This file provides code for the configuration
  *          of the DMA2D instances.
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

/* Includes ------------------------------------------------------------------*/
#include "dma2d.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DMA2D_HandleTypeDef hdma2d;

/* DMA2D init function */
void MX_DMA2D_Init(void)
{

	__HAL_RCC_DMA2D_CLK_ENABLE();         
	HAL_NVIC_SetPriority(DMA2D_IRQn,0,8);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);
	
//  hdma2d.Instance = DMA2D;
//  hdma2d.Init.Mode = DMA2D_M2M;
//  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
//  hdma2d.Init.OutputOffset = 0;
//  hdma2d.Init.BytesSwap = DMA2D_BYTES_REGULAR;
//  hdma2d.Init.LineOffsetMode = DMA2D_LOM_PIXELS;
//  hdma2d.LayerCfg[1].InputOffset = 0;
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
//  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
//  hdma2d.LayerCfg[1].InputAlpha = 0;
//  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
//  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
//  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
//  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
//  {
//    Error_Handler();
//  }

}

void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* dma2dHandle)
{

  if(dma2dHandle->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspInit 0 */

  /* USER CODE END DMA2D_MspInit 0 */
    /* DMA2D clock enable */
    __HAL_RCC_DMA2D_CLK_ENABLE();
  /* USER CODE BEGIN DMA2D_MspInit 1 */

  /* USER CODE END DMA2D_MspInit 1 */
  }
}

void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* dma2dHandle)
{

  if(dma2dHandle->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspDeInit 0 */

  /* USER CODE END DMA2D_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DMA2D_CLK_DISABLE();
  /* USER CODE BEGIN DMA2D_MspDeInit 1 */

  /* USER CODE END DMA2D_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

extern lv_disp_drv_t MyDisp_drv;


 void _DMA2D_Copy(void * pSrc, 
 void * pDst, 
 uint32_t xSize, 
 uint32_t ySize, 
 uint32_t OffLineSrc, 
 uint32_t OffLineDst, 
 uint32_t PixelFormat) 
 {
	 
 /* DMA2D 采用存储器到存储器模式, 这种模式是前景层作为 DMA2D 输入 */ 
 DMA2D->CR = 0x00000000UL | (1 << 9);   //开启传输完成中断
 DMA2D->FGMAR = (uint32_t)pSrc;
 DMA2D->OMAR = (uint32_t)pDst;
 DMA2D->FGOR = OffLineSrc;
 DMA2D->OOR = OffLineDst;

 /* 前景层和输出区域都采用的 RGB565 颜色格式 */
 DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
 DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

 DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
	 
 /* 启动传输 */
 DMA2D->CR |= DMA2D_CR_START; 

 /* 等待 DMA2D 传输完成 */
// while (DMA2D->CR & DMA2D_CR_START) {} 
// lv_disp_flush_ready(&MyDisp_drv);
	 
 }
 
 
 void LCD_FillRec(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color){
	 	SCB_CleanDCache();
	
    DMA2D->CR = 0x30000;
    DMA2D->OMAR = (uint32_t)(0xC0000000+(x0*2)+(y0*800*2));
    /*as input color mode is same as output we don't need to convert here do we?*/
	  DMA2D->FGPFCCR = 2;
    DMA2D->OPFCCR = 2;
	
    DMA2D->OCOLR = color;
    DMA2D->OOR = 800 - (x1-x0 +1);
    DMA2D->NLR = ( (uint32_t)(x1-x0 +1 ) << 16) | (y1-y0 +1);

    /*start transfer*/
    DMA2D->CR |= DMA2D_CR_START_Msk;

    while(DMA2D->CR & DMA2D_CR_START_Msk);
 }
 
 
 
 
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
