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


 
 /*		´«Í³·½Ê½	*/
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



///*		Ë«»º³å Ö±½Ó½»»»ÏÔ´æµØÖ·		*/
// static void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
//{
// 
//	SCB_CleanDCache();
//  HAL_LTDC_SetAddress(&hltdc, (uint32_t)color_p ,0);
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
	
	static lv_color_t  buf_1[800 * 40] __attribute__((section("axi_ram")));  //__attribute__ ((aligned (4)));
  static lv_color_t  buf_2[800 * 40] __attribute__((section("axi_ram")));  //__attribute__ ((aligned (4)));
	
	static lv_indev_drv_t indev_drv;
	static lv_disp_draw_buf_t disp_buf;
	static lv_fs_drv_t drv; 
	lv_disp_t * disp;
	
	/*·ÖÅä»º³åÇø*/ 
	lv_disp_draw_buf_init(&disp_buf,buf_1,buf_2,(800*40) );
	
  /*×¢²áÏÔÊ¾Æ÷*/
	lv_disp_drv_init(&MyDisp_drv); 
	MyDisp_drv.draw_buf = &disp_buf; 
	MyDisp_drv.flush_cb = my_flush_cb; 
	//MyDisp_drv.clean_dcache_cb = my_cleanDcache;
	MyDisp_drv.hor_res = 800; 
	MyDisp_drv.ver_res = 480; 
	disp = lv_disp_drv_register(&MyDisp_drv);
	
  /*×¢²á´¥ÃþÆÁ*/
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_drv_register(&indev_drv);
	
}





