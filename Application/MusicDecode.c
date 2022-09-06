/*
#################################################################################
  ��������Ƶ��������  
	���������� :  SPI2
	������Ӳ��IC��PCM 5102A(BB��˾)
	
																										CREATED: 2019/11/24, by ����
#################################################################################
*/
#include "MusicDecode.h"


 rt_mq_t mq_music;


// ----  DMA ���� �����ַ ��������жϣ�������ж� �ж�ʹ�� δ����   HAL_DMA_Start


uint8_t AudioBuffer0[1024*10] __attribute__ ((aligned (4)));//__attribute__((at(0xC1500000)));    //��Ƶ������0 

uint8_t AudioBuffer1[1024*10] __attribute__ ((aligned (4)));//__attribute__((at(0xC15186A0)));   //��Ƶ������1  



void AudioPlayer(void *path){
	
 RIFF *riff = NULL;
 FMT  *fmt  = NULL;
 Data *data = NULL;
	
 FIL AudioFil;
 FRESULT res;
 UINT btr = 0 , br = 0;
	
 uint32_t Audio_channels  = 0;            //������ or ������  ������ int or short
 int8_t  datasize        = -1;           //16bit or 24bit or 32bit
 uint32_t audiofrequency = 0;            //����Ƶ��
 uint8_t dmadatasize     = 0;            //0:8bit  1:16bit 2:32bit   24bit:��δ���
 uint16_t  dmanum        = 0;
 
 uint8_t mq_buff[100];
	
 mq_music = rt_mq_create("mq_music" , 100 , 10 , RT_IPC_FLAG_FIFO);
	
 if(mq_music == NULL ){
	 while(1); //����ʧ�� ��ֱ������ ���Ա����
 }
 restart:
		 memset(mq_buff , 0 , 100);
     rt_mq_recv(mq_music , mq_buff , 100 , RT_WAITING_FOREVER);
  
 new_music:
			__HAL_SAI_DISABLE(&hsai_BlockA1);
			__HAL_DMA_DISABLE(&hdma_sai1_a);
				
 
      btr = sizeof(RIFF) + sizeof(FMT) + sizeof(Data);
		
		  /*		���ļ� Ȩ��:����ģʽ		*/
			res = f_open(&AudioFil,(const char *)mq_buff,FA_READ);  
 
			/*		�����ļ����󣬹ر�	*/
			if(res != FR_OK){
					f_close(&AudioFil);
					goto restart;
			}
			
			/*		��ȡ��Ƶ�ļ��Ľṹ��Ϣ 		*/
			res = f_read(&AudioFil,AudioBuffer0,btr,&br);
			
			/*		�����ļ����󣬹ر�	*/
			if(res != FR_OK | (btr !=br )){
					f_close(&AudioFil);
					goto restart;
			}
			
			/*		�����ļ�����ȡ��Ƶ�ļ����ļ���Ϣ		*/
			riff = (RIFF *)(&AudioBuffer0[0]);
			fmt  = (FMT  *)(&AudioBuffer0[sizeof(RIFF)]);
			data = (Data *)(&AudioBuffer0[sizeof(RIFF)+sizeof(FMT)]);
			
			/*		�ж��Ƿ�Ϊ��׼WAV�ļ� ������wav���ر��ļ�		*/
			if(riff->File_type[0]!='W' || riff->File_type[1]!='A' || \
				 riff->File_type[2]!='V' || riff->File_type[3]!='E'){
					 
					 f_close(&AudioFil);
					 goto restart;
			}
			
			/*		��ȡ������ ��λ:Hz		*/	
			audiofrequency = fmt->Sample_rate;
			
			/*		��ȡͨ���� 		*/	
			if(fmt->Channel_Count == 1){
					Audio_channels = SAI_MONOMODE;
			}
			else if(fmt->Channel_Count == 2){
					Audio_channels = SAI_STEREOMODE;
			}
			else{
					;
			}
			
			/*		�ļ���Ϣ�еĲ���λ����HAL������	�ڴ�ת��	*/
			switch(fmt->BitsPerSample){
					case 8:
									datasize = -1;  //HAL�ݲ�֧��8bit
									dmadatasize = 0;
									dmanum = sizeof(AudioBuffer0);
									goto restart;
					case 16:
									datasize = SAI_PROTOCOL_DATASIZE_16BIT;
									dmadatasize = 1;
									dmanum = sizeof(AudioBuffer0)/2;
						      break;
					case 24:  // 24bit δ������
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

			/*		�ƶ��ļ�ָ�뵽PCM������		*/
			res |= f_lseek(&AudioFil,btr);
				
			/*		AudioBuffer0					*/
			res = f_read(&AudioFil,AudioBuffer0,sizeof(AudioBuffer0),&br);
			if(res != FR_OK){
				goto restart;
			}
			
			/*		���븴λSAI��ʹHALΪ����״̬������������������		*/
			HAL_SAI_DeInit(&hsai_BlockA1);
			
			/*		����SAI����			*/			
			SAI1_Config(Audio_channels,audiofrequency,datasize,2);   //����SAI
						
			/*		����DMA		*/ 

			ASI_DMA_DoubleBufferTransferStart((uint32_t)AudioBuffer0,(uint32_t)AudioBuffer1,dmadatasize,dmanum);
			
		 __HAL_SAI_ENABLE(&hsai_BlockA1);		 
				
	do{
			if(RT_EOK ==	rt_mq_recv(mq_music , mq_buff , 100 , 20)){
						if(mq_buff[0] == '0' && mq_buff[1] == ':'){  //DMA������ϢƵ�ʸ� ��˱��������ж������µ�music
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
						/*		�ر�������ļ�ϵͳ		*/
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
// uint8_t audiomode;              //��Ƶģʽ
// int8_t  datasize        = -1;   
// uint8_t dmadatasize     = 0;
// uint16_t  dmanum        = 0;
// uint32_t audiofrequency = 0;

//	
// btr = sizeof(RIFF) + sizeof(FMT) + sizeof(Data);
//		
//		/*			���ļ� Ȩ��:����ģʽ		*/
//			res = f_open(&AudioFil,(const char *)path,FA_READ);  
//			if(res != FR_OK){
//				return 1;
//			}
//			
//			/*		��ȡ��Ƶ�ļ��Ľṹ��Ϣ 		*/
//			res = f_read(&AudioFil,AudioBuffer0,btr,&br);
//			if(res != FR_OK | (btr !=br )){
//				goto error;
//			}
//			
//			/*		�����ļ�����ȡ��Ƶ�ļ����ļ���Ϣ		*/
//			riff = (RIFF *)(&AudioBuffer0[0]);
//			fmt  = (FMT  *)(&AudioBuffer0[sizeof(RIFF)]);
//			data = (Data *)(&AudioBuffer0[sizeof(RIFF)+sizeof(FMT)]);
//			
//			/*		�ж��Ƿ�Ϊ��׼WAV�ļ�		*/
//			if(riff->File_type[0]!='W' || riff->File_type[1]!='A' || \
//				 riff->File_type[2]!='V' || riff->File_type[3]!='E'){
//					 goto error;
//			}
//			
//			/*		��ȡ������ ��λ:Hz		*/	
//			audiofrequency = fmt->Sample_rate;
//			
//			/*		�ļ���Ϣ�еĲ���λ����HAL������	�ڴ�ת��	*/
//			switch(fmt->BitsPerSample){
//					case 8:
//									datasize = -1;  //HAL�ݲ�֧��8bit
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

//			/*		�ƶ��ļ�ָ�뵽PCM������		*/
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
//			/*		����SAI����			*/
//			HAL_SAI_DeInit(&hsai_BlockA1);						//��λSAI
//			SAI1_Config(0,audiofrequency,datasize,2);   //����SAI
//			
//			
//			/*		����DMA,SAI��DMA��32bit��FIFO����,ÿ�η�������,DMA������һ��word,�ʴ�������Ϊ 
//						�������ֽ�������4.
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










