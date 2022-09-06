/*
#################################################################################
  ···音频解码驱动  
	···外设 :  SPI2
	···硬件IC：PCM 5102A(BB公司)
	
																										CREATED: 2019/11/24, by 自由
#################################################################################
*/
#include "MusicDecode.h"


 rt_mq_t mq_music;


// ----  DMA 数量 外设地址 传输完成中断，半完成中断 中断使能 未配置   HAL_DMA_Start


uint8_t AudioBuffer0[1024*10] __attribute__ ((aligned (4)));//__attribute__((at(0xC1500000)));    //音频缓冲区0 

uint8_t AudioBuffer1[1024*10] __attribute__ ((aligned (4)));//__attribute__((at(0xC15186A0)));   //音频缓冲区1  



void AudioPlayer(void *path){
	
 RIFF *riff = NULL;
 FMT  *fmt  = NULL;
 Data *data = NULL;
	
 FIL AudioFil;
 FRESULT res;
 UINT btr = 0 , br = 0;
	
 uint32_t Audio_channels  = 0;            //单声道 or 立体音  必须用 int or short
 int8_t  datasize        = -1;           //16bit or 24bit or 32bit
 uint32_t audiofrequency = 0;            //采样频率
 uint8_t dmadatasize     = 0;            //0:8bit  1:16bit 2:32bit   24bit:暂未解决
 uint16_t  dmanum        = 0;
 
 uint8_t mq_buff[100];
	
 mq_music = rt_mq_create("mq_music" , 100 , 10 , RT_IPC_FLAG_FIFO);
	
 if(mq_music == NULL ){
	 while(1); //创建失败 ，直接阻塞 ，以便调试
 }
 restart:
		 memset(mq_buff , 0 , 100);
     rt_mq_recv(mq_music , mq_buff , 100 , RT_WAITING_FOREVER);
  
 new_music:
			__HAL_SAI_DISABLE(&hsai_BlockA1);
			__HAL_DMA_DISABLE(&hdma_sai1_a);
				
 
      btr = sizeof(RIFF) + sizeof(FMT) + sizeof(Data);
		
		  /*		打开文件 权限:仅读模式		*/
			res = f_open(&AudioFil,(const char *)mq_buff,FA_READ);  
 
			/*		操作文件有误，关闭	*/
			if(res != FR_OK){
					f_close(&AudioFil);
					goto restart;
			}
			
			/*		读取音频文件的结构信息 		*/
			res = f_read(&AudioFil,AudioBuffer0,btr,&br);
			
			/*		操作文件有误，关闭	*/
			if(res != FR_OK | (btr !=br )){
					f_close(&AudioFil);
					goto restart;
			}
			
			/*		解析文件，提取音频文件的文件信息		*/
			riff = (RIFF *)(&AudioBuffer0[0]);
			fmt  = (FMT  *)(&AudioBuffer0[sizeof(RIFF)]);
			data = (Data *)(&AudioBuffer0[sizeof(RIFF)+sizeof(FMT)]);
			
			/*		判断是否为标准WAV文件 ，若非wav，关闭文件		*/
			if(riff->File_type[0]!='W' || riff->File_type[1]!='A' || \
				 riff->File_type[2]!='V' || riff->File_type[3]!='E'){
					 
					 f_close(&AudioFil);
					 goto restart;
			}
			
			/*		获取采样率 单位:Hz		*/	
			audiofrequency = fmt->Sample_rate;
			
			/*		获取通道数 		*/	
			if(fmt->Channel_Count == 1){
					Audio_channels = SAI_MONOMODE;
			}
			else if(fmt->Channel_Count == 2){
					Audio_channels = SAI_STEREOMODE;
			}
			else{
					;
			}
			
			/*		文件信息中的采样位数与HAL不兼容	在此转换	*/
			switch(fmt->BitsPerSample){
					case 8:
									datasize = -1;  //HAL暂不支持8bit
									dmadatasize = 0;
									dmanum = sizeof(AudioBuffer0);
									goto restart;
					case 16:
									datasize = SAI_PROTOCOL_DATASIZE_16BIT;
									dmadatasize = 1;
									dmanum = sizeof(AudioBuffer0)/2;
						      break;
					case 24:  // 24bit 未经测试
									datasize = SAI_PROTOCOL_DATASIZE_24BIT;
									dmadatasize = 2;   
					        dmanum = sizeof(AudioBuffer0)/4;
						      break;
					case 32:
									datasize = SAI_PROTOCOL_DATASIZE_32BIT;
									dmadatasize = 2;
					        dmanum = sizeof(AudioBuffer0)/4;
						      break;
					default :
									goto restart;
			}	

			/*		移动文件指针到PCM数据区		*/
			res |= f_lseek(&AudioFil,btr);
				
			/*		AudioBuffer0					*/
			res = f_read(&AudioFil,AudioBuffer0,sizeof(AudioBuffer0),&br);
			if(res != FR_OK){
				goto restart;
			}
			
			/*		必须复位SAI，使HAL为重置状态，后续才能重新配置		*/
			HAL_SAI_DeInit(&hsai_BlockA1);
			
			/*		配置SAI外设			*/			
			SAI1_Config(Audio_channels,audiofrequency,datasize,2);   //配置SAI
						
			/*		配置DMA		*/ 

			ASI_DMA_DoubleBufferTransferStart((uint32_t)AudioBuffer0,(uint32_t)AudioBuffer1,dmadatasize,dmanum);
			
		 __HAL_SAI_ENABLE(&hsai_BlockA1);		 
				
	do{
			if(RT_EOK ==	rt_mq_recv(mq_music , mq_buff , 100 , 20)){
						if(mq_buff[0] == '0' && mq_buff[1] == ':'){  //DMA发送消息频率高 因此必须优先判断有无新的music
									f_close(&AudioFil);		
									goto new_music;
						}
						else if(strcmp((const char *)mq_buff , (const char *)MUSIC_PLAY) == 0){
									__HAL_SAI_ENABLE(&hsai_BlockA1);
									__HAL_DMA_ENABLE(&hdma_sai1_a);
						}
						else if(strcmp(mq_buff , MUSIC_PAUSE) == 0){
									__HAL_SAI_DISABLE(&hsai_BlockA1);
									__HAL_DMA_DISABLE(&hdma_sai1_a);
						}
						else if(strcmp((const char *)mq_buff , (const char *)MUSIC_SKIP) == 0){
							
						}
						else if(strcmp((const char *)mq_buff , (const char *)WRITE_TO_BUFF0)){
									res = f_read(&AudioFil,AudioBuffer0,sizeof(AudioBuffer0),&br);
							    if(res != FR_OK || f_eof(&AudioFil)){
										f_close(&AudioFil);
										goto restart;
									}
						}
						else if(strcmp((const char *)mq_buff , (const char *)WRITE_TO_BUFF1)){
									res = f_read(&AudioFil,AudioBuffer1,sizeof(AudioBuffer0),&br);
									if(res != FR_OK || f_eof(&AudioFil)){
										f_close(&AudioFil);
										goto restart;
									}
						}
						else{
							    goto restart;
						}
						/*		关闭外设和文件系统		*/
			}
	}while(1);

	return;
}









//char AudioPlayer(void *path){
//	
// RIFF *riff = NULL;
// FMT  *fmt  = NULL;
// Data *data = NULL;
// FIL AudioFil;
// static FRESULT res;
// UINT btr = 0 , br = 0;
//	
// uint8_t audiomode;              //音频模式
// int8_t  datasize        = -1;   
// uint8_t dmadatasize     = 0;
// uint16_t  dmanum        = 0;
// uint32_t audiofrequency = 0;

//	
// btr = sizeof(RIFF) + sizeof(FMT) + sizeof(Data);
//		
//		/*			打开文件 权限:仅读模式		*/
//			res = f_open(&AudioFil,(const char *)path,FA_READ);  
//			if(res != FR_OK){
//				return 1;
//			}
//			
//			/*		读取音频文件的结构信息 		*/
//			res = f_read(&AudioFil,AudioBuffer0,btr,&br);
//			if(res != FR_OK | (btr !=br )){
//				goto error;
//			}
//			
//			/*		解析文件，提取音频文件的文件信息		*/
//			riff = (RIFF *)(&AudioBuffer0[0]);
//			fmt  = (FMT  *)(&AudioBuffer0[sizeof(RIFF)]);
//			data = (Data *)(&AudioBuffer0[sizeof(RIFF)+sizeof(FMT)]);
//			
//			/*		判断是否为标准WAV文件		*/
//			if(riff->File_type[0]!='W' || riff->File_type[1]!='A' || \
//				 riff->File_type[2]!='V' || riff->File_type[3]!='E'){
//					 goto error;
//			}
//			
//			/*		获取采样率 单位:Hz		*/	
//			audiofrequency = fmt->Sample_rate;
//			
//			/*		文件信息中的采样位数与HAL不兼容	在此转换	*/
//			switch(fmt->BitsPerSample){
//					case 8:
//									datasize = -1;  //HAL暂不支持8bit
//									dmadatasize = 0;
//									goto error;
//						      break;
//					case 16:
//									datasize = SAI_PROTOCOL_DATASIZE_16BIT;
//									dmadatasize = 1;
//						      break;
//					case 24:
//									datasize = SAI_PROTOCOL_DATASIZE_24BIT;
//						      break;
//					case 32:
//									datasize = SAI_PROTOCOL_DATASIZE_32BIT;
//									dmadatasize = 2;
//						      break;
//					default :
//									goto error;
//						      break;
//			}	

//			/*		移动文件指针到PCM数据区		*/
//			res = f_lseek(&AudioFil,btr);
//			if(res != FR_OK){
//				goto error;
//			}
//				
//			/*		AudioBuffer0					*/
//			res = f_read(&AudioFil,AudioBuffer0,sizeof(AudioBuffer0),&br);
//			if(res != FR_OK){
//				goto error;
//			}
//			
//			/*		配置SAI外设			*/
//			HAL_SAI_DeInit(&hsai_BlockA1);						//复位SAI
//			SAI1_Config(0,audiofrequency,datasize,2);   //配置SAI
//			
//			
//			/*		配置DMA,SAI的DMA由32bit的FIFO管理,每次发起请求,DMA将传输一个word,故传输数量为 
//						缓冲区字节数除以4.
//			*/
//			ASI_DMA_DoubleBufferTransferStart(AudioBuffer0,AudioBuffer1,dmadatasize,(sizeof(AudioBuffer0)/2));
//			
//		 __HAL_SAI_ENABLE(&hsai_BlockA1);
//										
//			do{
//					while(DMA1_Stream0_Cplt_status>2){
//						rt_thread_mdelay(5);
//					}
//					if(DMA1_Stream0_Cplt_status==1){
//						 res = f_read(&AudioFil,AudioBuffer0,sizeof(AudioBuffer0),&br);
//							if(res != FR_OK){
//								goto error;
//							}
//					 }
//					 else{	 
//						 res = f_read(&AudioFil,AudioBuffer1,sizeof(AudioBuffer0),&br);
//						 if(res != FR_OK){
//								goto error;
//						}
//					 }
//					DMA1_Stream0_Cplt_status = 5;
//			}while(br == sizeof(AudioBuffer0));

//	error:
//			__HAL_SAI_DISABLE(&hsai_BlockA1);
//			__HAL_DMA_DISABLE(&hdma_sai1_a);
//			f_close(&AudioFil);
//	return res;
//}










