/**
 * @file lvglBsp.c
 * Configuration file for v8.1.0-dev
 */
 
 #include "lvglBsp.h"
  #include "lv_hal_disp.h"
 #include "ltdc.h"
 #include "gt9147.h"
 #include "string.h"
 #include "stdlib.h"
 #include <stdbool.h>
 #include "ff.h"
 #include "stm32h7xx_hal_dma2d.h"
 
 static void* MyFileSystemOpenFunCallBack(struct _lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
 static lv_fs_res_t MyFileSystemCloseFunCallBack(struct _lv_fs_drv_t * drv, void * file_p);
 static bool MyFileSystemReadyFunCallBack(struct _lv_fs_drv_t * drv);
 static lv_fs_res_t MyFileSystemReadFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
 static lv_fs_res_t MyFileSystemWriteFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
 static lv_fs_res_t MyFileSystemSeekFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
 static lv_fs_res_t MyFileSystemTellFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
 static void* MyFileSystemOpenDirFunCallBack(struct _lv_fs_drv_t * drv, const char * path);
 static lv_fs_res_t MyFileSystemReadDirFunCallBack(struct _lv_fs_drv_t * drv, void * rddir_p, char * fn);
 static lv_fs_res_t MyFileSystemCloseDirFunCallBack(struct _lv_fs_drv_t * drv, void * rddir_p);
 
 
 
 extern DMA2D_HandleTypeDef hdma2d;
 extern LTDC_HandleTypeDef hltdc;
 extern _m_tp_dev tp_dev;
 
 extern  void _DMA2D_Copy(void * pSrc, 
 void * pDst, 
 uint32_t xSize, 
 uint32_t ySize, 
 uint32_t OffLineSrc, 
 uint32_t OffLineDst, 
 uint32_t PixelFormat
 );
 


 lv_disp_drv_t MyDisp_drv; 


 
 /*		传统方式	*/
 static void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	SCB_CleanDCache();
  _DMA2D_Copy(
	color_p,
	(0xC0000000+((area->x1)*2)+((area->y1)*800*2)),
	(area->x2 - area->x1 + 1),
	(area->y2 - area->y1 + 1),
	0,
	800-(area->x2 - area->x1 + 1),
	LTDC_PIXEL_FORMAT_RGB565
	);
//	LCD_Draw_Rectangle(area->x1,area->y1,area->x2,area->y2,color_p);
//	
//	lv_disp_flush_ready(&MyDisp_drv);
}



///*		双缓冲 直接交换显存地址		*/
// static void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
//{
//	//SCB_DisableICache();
//  //SCB_DisableDCache();
//  HAL_LTDC_SetAddress(&hltdc, (uint32_t)color_p ,0);
//	//SCB_EnableICache();
//  //SCB_EnableDCache();
//	lv_disp_flush_ready(disp_drv);
//}


static void my_cleanDcache(struct _lv_disp_drv_t * disp_drv){
	
	SCB_CleanDCache();
}


	
void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
		if(tp_dev.sta&(0x01<<15)) {
				data->point.x = tp_dev.x[0];
				data->point.y = tp_dev.y[0];
				data->state = LV_INDEV_STATE_PRESSED;
		} 
		else {
			  data->point.x = tp_dev.x[4];
				data->point.y = tp_dev.y[4];
				data->state = LV_INDEV_STATE_RELEASED;
		}
}


	

void LvgBspInit(){
	
	static lv_color_t  buf_1[800 * 100] __attribute__ ((aligned (4)));
  static lv_color_t  buf_2[800 * 100] __attribute__ ((aligned (4)));
	static lv_indev_drv_t indev_drv;
	static lv_disp_draw_buf_t disp_buf;
	static lv_fs_drv_t drv; 
	lv_disp_t * disp;
	
	/*分配缓冲区*/ 
	lv_disp_draw_buf_init(&disp_buf,buf_1,buf_2,(800*100));
	
  /*注册显示器*/
	lv_disp_drv_init(&MyDisp_drv); 
	MyDisp_drv.draw_buf = &disp_buf; 
	//MyDisp_drv.full_refresh = 1;
	MyDisp_drv.flush_cb = my_flush_cb; 
	//MyDisp_drv.clean_dcache_cb = my_cleanDcache;
	MyDisp_drv.hor_res = 800; 
	MyDisp_drv.ver_res = 480; 
	disp = lv_disp_drv_register(&MyDisp_drv);
	
  /*注册触摸屏*/
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_drv_register(&indev_drv);
	
	/*注册文件系统*/
	//lv_fs_if_init();
	lv_fs_drv_init(&drv); 
	drv.letter = 'S'; 
	drv.ready_cb = MyFileSystemReadyFunCallBack;
	drv.open_cb = MyFileSystemOpenFunCallBack; 
	drv.close_cb = MyFileSystemCloseFunCallBack; 
	drv.read_cb = MyFileSystemReadFunCallBack;
	drv.write_cb = MyFileSystemWriteFunCallBack; 
	drv.seek_cb = MyFileSystemSeekFunCallBack; 
	drv.tell_cb = MyFileSystemTellFunCallBack; 
	drv.dir_open_cb = MyFileSystemOpenDirFunCallBack; 
	drv.dir_read_cb = MyFileSystemReadDirFunCallBack; 
	drv.dir_close_cb = MyFileSystemCloseDirFunCallBack;
	drv.user_data = NULL; 
	lv_fs_drv_register(&drv);
}



/*		利用Fatfs实现LVGL的文件接口*/

static void* MyFileSystemOpenFunCallBack(struct _lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode){
  
	uint8_t flags = 0;
	FRESULT res;
	FIL *pFile=NULL;
	pFile = (FIL *)malloc(sizeof(FIL));   
	if(pFile==NULL){
		return NULL;
	}
	memset(pFile,0,sizeof(FIL));
  if(mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
  else if(mode == LV_FS_MODE_RD) flags = FA_READ;
  else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
  res = f_open(pFile, path, flags);
  if(res == FR_OK) {
    	res = f_lseek(pFile, 0);
		  if(res == FR_OK){
					return pFile;
			}
			else{
				while(1); //移动文件指针失败，阻塞程序，以便查错
			}
   }
	 else {
		  free(pFile);
    	return LV_FS_RES_UNKNOWN;
   }
}
		

static lv_fs_res_t MyFileSystemCloseFunCallBack(struct _lv_fs_drv_t * drv, void * file_p){
	
	FRESULT res;  
	res = f_close(file_p);
	free(file_p);
	if(res == FR_OK){
    return LV_FS_RES_OK;
	}
	else{
		while(1);  //如果关闭文件失败，阻塞程序，以便快速定位
	}
}


static bool MyFileSystemReadyFunCallBack(struct _lv_fs_drv_t * drv){
	return true;  //暂不处理
}


static lv_fs_res_t MyFileSystemReadFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br){
	 volatile FRESULT res = 0;
	 res= f_read(file_p, buf, btr, (UINT*)br);
  if(res == FR_OK){ 
		res = 0;
		return res;
		//return LV_FS_RES_OK;
	}
  else{ 
		while(1); //如果读取文件失败，阻塞程序，以便快速定位
		return LV_FS_RES_UNKNOWN;
	}
}


static lv_fs_res_t MyFileSystemWriteFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw){
	FRESULT res = f_write(file_p, buf, btw, (UINT*)bw);
  if(res == FR_OK){
		return LV_FS_RES_OK;
	}
  else {
		while(1);  //如果写入文件失败，阻塞程序，以便快速定位
		return LV_FS_RES_UNKNOWN;
	}
}


static lv_fs_res_t MyFileSystemSeekFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence){
	  
switch (whence)
    {
    case LV_FS_SEEK_SET:
        f_lseek(file_p, pos);
        break;
    case LV_FS_SEEK_CUR:
        f_lseek(file_p, f_tell((FIL *)file_p) + pos);
        break;
    case LV_FS_SEEK_END:
        f_lseek(file_p, f_size((FIL *)file_p) + pos);
        break;
    default:
        break;
    }
    return LV_FS_RES_OK;
}


static lv_fs_res_t MyFileSystemTellFunCallBack(struct _lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p){
		
	 *pos_p = f_tell(((FIL *)file_p));
   return LV_FS_RES_OK;
}




static void* MyFileSystemOpenDirFunCallBack(struct _lv_fs_drv_t * drv, const char * path){
	
	static FRESULT res;
	DIR *pDir=NULL;
	pDir = (DIR *)malloc(sizeof(DIR));   
	if(pDir==NULL){
		return NULL;
	}
	memset(pDir,0,sizeof(DIR));
  res = f_opendir(pDir, path);
  if(res == FR_OK){
		return pDir;
	}
  else{
		while(1);  //如果文件夹打开失败，阻塞程序，以便快速定位
		return LV_FS_RES_UNKNOWN;
	}
}


static lv_fs_res_t MyFileSystemReadDirFunCallBack(struct _lv_fs_drv_t * drv, void * rddir_p, char * fn){
	
	FRESULT res;
	FILINFO fno;
	fn[0] = '\0';
  do{
			res = f_readdir(rddir_p, &fno);
				if(res != FR_OK) {
					return LV_FS_RES_UNKNOWN;
				}
				if(fno.fattrib & AM_DIR) {
				fn[0] = '/';
				strcpy(&fn[1], fno.fname);
			}
			else{
				strcpy(fn, fno.fname);
			}
   }while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);
    
	 return LV_FS_RES_OK;
}



static lv_fs_res_t MyFileSystemCloseDirFunCallBack(struct _lv_fs_drv_t * drv, void * rddir_p){
	  
	f_closedir(rddir_p);
	free(rddir_p);
  return LV_FS_RES_OK;
}









