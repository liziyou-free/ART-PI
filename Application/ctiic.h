#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "stm32h7xx_hal.h"

//IO方向设置
#define CT_SDA_IN()  {GPIOB->MODER&=~(3<<(2*7));GPIOB->MODER|=0<<2*7;}	//PF7输入模式
#define CT_SDA_OUT() {GPIOB->MODER&=~(3<<(2*7));GPIOB->MODER|=1<<2*7;} 	//PF7输出模式
//IO操作函数	 
#define CT_IIC_SCL(n) (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET))//SCL
#define CT_IIC_SDA(n) (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET))//SDA	 
#define CT_READ_SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)//输入SDA 
 

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
uint8_t CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif







