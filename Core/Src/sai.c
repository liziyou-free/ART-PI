/**
  ******************************************************************************
  * File Name          : SAI.c
  * Description        : This file provides code for the configuration
  *                      of the SAI instances.
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
#include "sai.h"

#include "gpio.h"
#include "dma.h"

SAI_HandleTypeDef hsai_BlockA1;
DMA_HandleTypeDef hdma_sai1_a;


/* USER CODE BEGIN 0 */

/*			根据Cube生成代码修改	*/
void SAI1_Config(uint32_t mode,uint32_t Frequncy,uint8_t Datasize,uint8_t slots)
{
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = mode;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_BlockA1.Init.AudioFrequency = Frequncy;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, Datasize, slots) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN SAI1_Init 2 */			
 
  /* USER CODE END SAI1_Init 2 */

}


/*			根据Cube生成代码修改	*/
/*
	Buf0: 缓冲区0的地址
  Buf1: 缓冲区1的地址
	size：数据尺寸 0:8bit 1:16bit 2:32bit  
  num ：DMA传输次数
*/
void ASI_DMA_DoubleBufferTransferStart(uint32_t Buf0,uint32_t Buf1,uint32_t size,uint32_t num){
	
	  volatile uint32_t temp = 0;
	
		/* cubeMX \官方HAL库 皆未发现双buffer相关配置接口	故,直接操作寄存器*/   
	
     __HAL_DMA_DISABLE(&hdma_sai1_a);    //失能DMA_stream0
	
		((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->CR |= (1<<18);     //打开双缓冲
	  temp = (size<<11);
	  ((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->CR &= ~(3<<11);
	  ((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->CR |= temp;  //配置外设数据大小
		temp = (size<<13);
		((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->CR &= ~(3<<13);
	  ((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->CR |= temp;  //配置储存区数据大小
		((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->M0AR = Buf0;
		((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->M1AR = Buf1;
	  ((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->NDTR = num;
	  ((DMA_Stream_TypeDef*)(hdma_sai1_a.Instance))->PAR = 	
						(uint32_t)&(((SAI_Block_TypeDef *)(hsai_BlockA1.Instance))->DR);
	
	/*	清除所有中断请求 然后仅打开DMA_IT_TC(传输完成中断) */
	 ((DMA_Stream_TypeDef *)(hdma_sai1_a.Instance))->CR &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME | DMA_IT_HT);
	 ((DMA_Stream_TypeDef *)(hdma_sai1_a.Instance))->CR |=	(DMA_IT_TC); 			
	  ((SAI_Block_TypeDef *)hsai_BlockA1.Instance)->CR1 |= SAI_xCR1_DMAEN; //使能SAI1_blockA的DMA请求 
	  
	__HAL_DMA_ENABLE(&hdma_sai1_a);      //使能DMA_stream0
	
		/*   				寄存器配置结束						*/
}


/* USER CODE END 0 */









//SAI_HandleTypeDef hsai_BlockA1;
//DMA_HandleTypeDef hdma_sai1_a;

/* SAI1 init function */
void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */

  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_44K;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN SAI1_Init 2 */
  ((SAI_Block_TypeDef *)hsai_BlockA1.Instance)->CR1 |= SAI_xCR1_DMAEN; //使能SAI1_blockA的DMA请求 
  /* USER CODE END SAI1_Init 2 */

}




static uint32_t SAI1_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct;
/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    /* SAI1 clock enable */
    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client ++;

    /**SAI1_A_Block_A GPIO Configuration
    PE5     ------> SAI1_SCK_A
    PE4     ------> SAI1_FS_A
    PE6     ------> SAI1_SD_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_sai1_a.Instance = DMA1_Stream0;
    hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		//hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a.Init.FIFOThreshold = DMA_FIFOMODE_DISABLE;
    hdma_sai1_a.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
		
    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      Error_Handler();
    }
		
    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(hsai,hdmarx,hdma_sai1_a);
    __HAL_LINKDMA(hsai,hdmatx,hdma_sai1_a);
    }
}


void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{

/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    SAI1_client --;
    if (SAI1_client == 0)
      {
      /* Peripheral clock disable */
       __HAL_RCC_SAI1_CLK_DISABLE();
      }

    /**SAI1_A_Block_A GPIO Configuration
    PE5     ------> SAI1_SCK_A
    PE4     ------> SAI1_FS_A
    PE6     ------> SAI1_SD_A
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_6);

    HAL_DMA_DeInit(hsai->hdmarx);
    HAL_DMA_DeInit(hsai->hdmatx);
    }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
