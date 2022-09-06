
#pragma once
#include "stm32h7xx_hal.h"
#include "sai.h"
#include "ff.h"
#include "rtthread.h"

/*		UI �� DMA �� Music �໥ͨѶ�Ķ�����Ϣ����		*/
#define  MUSIC_PLAY  					"m-play"				//����
#define  MUSIC_PAUSE  				"m-pause"  			//��ͣ
#define  MUSIC_SKIP  					"m-skip"				//��ת
#define  MUSIC_KILL						"m-kill"				//����
#define  WRITE_TO_BUFF0				"w-buff0"				//��PCMд��buff0
#define  WRITE_TO_BUFF1				"w-buff1"				//��PCMд��buff1
			
     
	   
//wav ��Ƶ�ļ��ṹ
typedef struct {
  uint8_t File_Flag[4];   			//�ĵ���ʶ
  uint32_t Data_Length;  		  //�ӵ�ǰλ�õ��ļ�ĩβ���ļ��ֽ��� (��ˣ������ļ���СҪ�ٴλ�������+8byte)
  uint8_t File_type[4];  		  //�ļ���ʽ���� �˴�Ӧ�ú�Ϊ�ַ��� "WAVE" 
}RIFF;

typedef struct {
  uint8_t File_Flag[4];        //��ʽ���ʶ   �˴�Ӧ�ú�Ϊ�ַ��� "fmt "   (fmt+�ո�) 
  uint32_t Block_Length;       //��ʽ�鳤��
	uint16_t Coding_Num;         //�������
	uint16_t Channel_Count;      //��������
	uint32_t Sample_rate;        //������
	uint32_t Bit_Rate;           //ÿ�봫��������� ��ֵΪͨ������ÿ����������ÿ������������λ��/8
	uint16_t Block_Align;        //����� ��ʾһ����������Ƶռ���ֽ��� 
	uint16_t BitsPerSample;      //����λ�� ���磺8bit��16bit��24bit��32bit
}FMT;

typedef struct {
	uint8_t  String_buf[4];   		//��Ϊ�ַ����� "data"   ��ʾͷ�ļ������Ѿ�����
	uint32_t VoiceData_Size;     //��Ƶ���ݴ�С ��������Ƶ���ݵ��ֽ�������
}Data;



void AudioPlayer(void *path);


extern  rt_mq_t mq_music;





