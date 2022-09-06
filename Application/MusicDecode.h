
#pragma once
#include "stm32h7xx_hal.h"
#include "sai.h"
#include "ff.h"
#include "rtthread.h"

/*		UI 、 DMA 、 Music 相互通讯的队列消息定义		*/
#define  MUSIC_PLAY  					"m-play"				//播放
#define  MUSIC_PAUSE  				"m-pause"  			//暂停
#define  MUSIC_SKIP  					"m-skip"				//跳转
#define  MUSIC_KILL						"m-kill"				//结束
#define  WRITE_TO_BUFF0				"w-buff0"				//将PCM写入buff0
#define  WRITE_TO_BUFF1				"w-buff1"				//将PCM写入buff1
			
     
	   
//wav 音频文件结构
typedef struct {
  uint8_t File_Flag[4];   			//文档标识
  uint32_t Data_Length;  		  //从当前位置到文件末尾的文件字节数 (因此，整个文件大小要再次基础上再+8byte)
  uint8_t File_type[4];  		  //文件格式类型 此处应该恒为字符串 "WAVE" 
}RIFF;

typedef struct {
  uint8_t File_Flag[4];        //格式块标识   此处应该恒为字符串 "fmt "   (fmt+空格) 
  uint32_t Block_Length;       //格式块长度
	uint16_t Coding_Num;         //编码代号
	uint16_t Channel_Count;      //声道数量
	uint32_t Sample_rate;        //采样率
	uint32_t Bit_Rate;           //每秒传输的数据量 其值为通道数×每秒样本数×每个样本的数据位数/8
	uint16_t Block_Align;        //块对齐 表示一个声道的音频占的字节数 
	uint16_t BitsPerSample;      //采样位数 例如：8bit、16bit、24bit、32bit
}FMT;

typedef struct {
	uint8_t  String_buf[4];   		//恒为字符串： "data"   表示头文件数据已经结束
	uint32_t VoiceData_Size;     //音频数据大小 （整个音频数据的字节数量）
}Data;



void AudioPlayer(void *path);


extern  rt_mq_t mq_music;





