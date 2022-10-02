#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "ltdc.h"
#include "quadspi.h"
#include "sai.h"
#include "sdmmc.h"
#include "gpio.h"
#include "fmc.h"
#include "sockets.h"
#include "lwip.h"
#include "MusicDecode.h"

uint8_t *FontBuffer;

void SystemClock_Config(void);
static void MPU_Config( void );
void Touch_scan(void *para);
void LvglTask(void *para);
void LedTask(void *p);
void AudioTask(void *para);
void NetTask(void *para);

int main(void)
{
    rt_thread_t task0;
    rt_thread_t task1;
    rt_thread_t task2;
    rt_thread_t task3;
    rt_thread_t task4;

    MPU_Config();
    SCB_EnableICache();
    SCB_EnableDCache();  

    /*  QSPI_flash初始化 并映射  */
    W25QXX_Init();
    W25Q_Memory_Mapped_Enable();

    //MX_GPIO_Init();  //__mian 之前进行了初始化 因为fmc依赖GPIO初始化.
    MX_DMA2D_Init();
    MX_DMA_Init();
    MX_LTDC_Init();
    //MX_FMC_Init();  //__mian 之前进行了初始化 目的：将heap定位到外部sdram,从而使用malloc管理sdram.
    MX_SDMMC1_SD_Init();
    MX_FATFS_Init();
    MX_SAI1_Init();
    MX_LWIP_Init();

//    task0 = rt_thread_create("Lvgl",LvglTask,0,1024*4,22,0);
//    rt_thread_startup(task0);

//    task1 = rt_thread_create("Touch_scan",Touch_scan,0,1024,21,0);
//    rt_thread_startup(task1);

    task2 = rt_thread_create("Led",LedTask,0,512,20,0);
    rt_thread_startup(task2);

//    task3 = rt_thread_create("Music",AudioTask,"0:test.wav",1024*2,5,0);
//    rt_thread_startup(task3);

    task4 = rt_thread_create("Net",NetTask,0,1024*2,4,0);
    rt_thread_startup(task4);

  while (1)
  {
    return 0;
  }
}




void Touch_scan(void *para){

    GT9147_Init();
    while(true){
    //lv_tick_inc(1);
     GT9147_Scan(0);
     rt_thread_mdelay(10);
    }
}



void LvglTask(void *para){

    #include "lv_disp.h"
    extern  lv_disp_drv_t MyDisp_drv;

    static FIL  ExFont24 __attribute__((section("axi_ram"))); //必须定位到axi-ram 否则可能读写失败
    static UINT br;
    static FRESULT res;
    static uint32_t io_time = 0;

//    /*  LVGL font24 加载到 SDRAM   5.6MB   */
//    FontBuffer = malloc(1024*1024*6);
//    res |= f_mount(&SDFatFS, "0", 0);
//    res |= f_open(&ExFont24 , "0:/LvglFont/Font24.bin" , FA_READ);
//    io_time = HAL_GetTick();
//    res |= f_read(&ExFont24 , FontBuffer , f_size(&ExFont24) , &br);
//    io_time = HAL_GetTick() - io_time;
//    res |= f_close(&ExFont24);
//    if(res != FR_OK){
//        io_time = 0;
//        while(1);
//    }

    LCD_Fill(0,0,800,480,0);
    lv_init();
    LvgBspInit();
    lv_timer_t *time = _lv_disp_get_refr_timer(NULL);
    lv_timer_set_period(time, 3);

    lv_demo_benchmark();
    //lv_demo_widgets();
    //lv_demo_music();

    //lv_png_init();
    //CreateMyLvglDemo();
    while(true){
        lv_task_handler();
        rt_thread_mdelay(1);
    }
}


uint8_t *NetBuf ;

static int SendUtilEnd(int s, char*dataptr, size_t size){

    int size_done = 0;

    do{
        size_done = write(s,dataptr, size);
        if(size_done<0)
        {
                return -1;      //发生错误
        }
        dataptr += size_done;
        size = size-size_done;
        rt_thread_mdelay(3);
    }while(size>0);
     
   return 0;
}

int SendAllData(int s, char*dataptr, size_t size)
{
    #define unit_bytes     1024
    
    int res;

    do{
        if(size > unit_bytes)
        {
                res = SendUtilEnd(s,dataptr,unit_bytes);
                dataptr += unit_bytes;
                size -= unit_bytes;
        }
        else
        {
                res = SendUtilEnd(s,dataptr,size);
                size = 0;
        }
        
        if(res == -1)
        {
                return -1;
        }
        
        rt_thread_mdelay(3);
            
    }while(size);
    
    return 0;
}




extern struct netif gnetif;
static int ccc;

void NetTask(void *para){

    int sct = 0; 
    int client_sct = 0;
    int res = 0;
    struct sockaddr_in ip ;
    struct sockaddr_in client_ip ;
    memset(&ip,0,sizeof(struct sockaddr_in));

    while(!netif_is_link_up(&gnetif))
    {
        rt_thread_mdelay(200);
    }

    NetBuf= malloc(1024*200);
    if(!NetBuf){
        while(1);
    }
    uint8_t *dtr = "Hello,word!@FreedomLi";
    memcpy(NetBuf, dtr, strlen(dtr));
    NetBuf[1024*10-1] = '%';
restart:
    sct = socket(AF_INET,SOCK_DGRAM,0);
  if(sct<0){    //创建失败
    close(sct);
    goto restart;
  }
  
    ip.sin_port = htons(666);
    ip.sin_family = AF_INET;
    ip.sin_addr.s_addr = inet_addr("192.168.0.8");
  
//  res = connect(sct,(struct sockaddr*)&ip,sizeof(struct sockaddr_in));
//  res |= bind(sct,(struct sockaddr*)&ip,sizeof(struct sockaddr_in));
//  res |= listen(sct,10);
//  if(res<0){
//    close(sct);
//  rt_thread_mdelay(2);
//    goto restart;
//  }
  int len = sizeof(struct sockaddr_in);
//  client_sct = accept(sct, &client_ip, (socklen_t *)&len);
 
  
  client_ip.sin_port = htons(888);
  client_ip.sin_family = AF_INET;
  client_ip.sin_addr.s_addr = inet_addr("192.168.0.4");
  char *dtr0 = "Hello,Word and freeli, good evening!";
  
   rt_thread_mdelay(3000); 
   
    while(1){
//  res = recvfrom(sct, &NetBuf[1024], 1024*10, MSG_DONTWAIT, &client_ip, &len);
//      if(res<0xfffff && res>=0){
    
//   ccc =  recvfrom(sct, &NetBuf[1024], 1024*10, MSG_DONTWAIT, &client_ip, &len);
     
        ccc = sendto(sct, dtr0, 1024*50-1, MSG_WAITALL, &client_ip, len);
//        if( SendAllData(client_sct,"Hello,Word and freeli, good evening!", 36 )){
//            close(client_sct);
//            close(sct);
//       getpeername(sct, &client_ip, (socklen_t *)&len);
            rt_thread_mdelay(10); 
//            goto restart;
//      }
//      res = 0;
    //}
    }
}


void AudioTask(void *para){
    while(true){
     //AudioPlayer(para);
     rt_thread_mdelay(100);
    }
}


void LedTask(void *p){

    static uint8_t status = 0;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);

    /*Configure GPIO pin : PI8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    while(true){
        status = !status;
        HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, status);
        rt_thread_mdelay(200);
    }
}

//void HAL_Delay(uint32_t Delay)
//{
//  rt_thread_mdelay(Delay);
//}


void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    /** Macro to configure the PLL clock source
    */
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
    Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI1
                              |RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_QSPI
                              |RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.PLL3.PLL3M = 32;
    PeriphClkInitStruct.PLL3.PLL3N = 192;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 2;
    PeriphClkInitStruct.PLL3.PLL3R = 7;  //
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;// RCC_FMCCLKSOURCE_PLL;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
}


static void MPU_Config( void )
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    HAL_MPU_Disable();

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x24000000;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0xC0000000;
    MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_32MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x90000000;
    MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_8MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;	
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x80000000;
    MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_2MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    /* Configure the MPU attributes as Normal Non Cacheable
     for LwIP RAM heap which contains the Tx buffers */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30020000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER4;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as Device not cacheable 
     for ETH DMA descriptors */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30040000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER5;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x20000000;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_128KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER6;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

//  ARM_LIB_HEAP  0xC0100000 UNINIT 0x2000000 { ; 

//  .ANY (HEAP)                                        
//  }