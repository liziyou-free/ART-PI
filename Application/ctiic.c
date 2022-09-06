#include "ctiic.h"
#include "stm32h7xx_hal.h"	  	

//����I2C�ٶȵ���ʱ
void CT_Delay(void)
{
	int t=1000;
	while(t--);
} 

void CT_Delay1(int time)
{
	int t=100*time;
	while(t--);
} 


//���ݴ���оƬIIC�ӿڳ�ʼ��
void CT_IIC_Init(void)
{					     
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_6;            //PB6
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;    //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��
	
    GPIO_Initure.Pin=GPIO_PIN_7;            //PB7
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��
}
//����IIC��ʼ�ź�
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda�����
	CT_IIC_SDA(1);	  	  
	CT_IIC_SCL(1);
	CT_Delay1(30);
 	CT_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL(0); 
	CT_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
	CT_Delay1(30);
	CT_IIC_SCL(1); 
	CT_Delay(); 
	CT_IIC_SDA(1);//����I2C���߽����ź�  
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����  
	CT_IIC_SDA(1);	   
	CT_IIC_SCL(1);
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL(0);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void CT_IIC_Ack(void)
{
	CT_IIC_SCL(0);
	CT_SDA_OUT(); 
	CT_IIC_SDA(0);
	CT_Delay();
	CT_IIC_SCL(1);
	CT_Delay();
	CT_IIC_SCL(0);
}
//������ACKӦ��		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL(0);
	CT_SDA_OUT(); 
	CT_IIC_SDA(1);
	CT_Delay();
	CT_IIC_SCL(1);
	CT_Delay();
	CT_IIC_SCL(0);
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	CT_SDA_OUT(); 	    
    CT_IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ��� 
	for(t=0;t<8;t++)
    {              
        CT_IIC_SDA((txd&0x80)>>7);
        txd<<=1; 
		CT_Delay();	      
		CT_IIC_SCL(1); 
		CT_Delay();
		CT_IIC_SCL(0);	 
		CT_Delay();
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ���� 
	CT_Delay1(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL(0); 	    	   
		CT_Delay();
		CT_IIC_SCL(1);	 
		receive<<=1;
		if(CT_READ_SDA)receive++; 
		CT_Delay();  
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}




























