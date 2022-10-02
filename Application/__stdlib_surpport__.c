
/*
		实现C库的互斥锁接口，以便部分C库函数支持线程安全和可重入

*/

#include "rtthread.h"
#include "mem_with_mutex.h"

extern char  rtthread_is_init;

rt_mutex_t  __g_malloc_mutex;


//void *__malloc(size_t s/*size*/){

//  void *p_addr;
//  
//  rt_mutex_take(__g_malloc_mutex, RT_WAITING_FOREVER);
//  p_addr = malloc(s);
//  rt_mutex_release(__g_malloc_mutex);
//  return p_addr;
//}

//void *__realloc(void * p_ptr/*ptr*/, size_t s/*size*/){
//  
//  void *p_addr;
//  
//  rt_mutex_take(__g_malloc_mutex, RT_WAITING_FOREVER);
//  p_addr = realloc(p_ptr, s);
//  rt_mutex_release(__g_malloc_mutex);
//}

//void __free(void * p_ptr/*ptr*/){
//  
//  rt_mutex_take(__g_malloc_mutex, RT_WAITING_FOREVER);
//  free(p_ptr);
//  rt_mutex_release(__g_malloc_mutex);
//}

//void *__calloc(size_t s0/*nmemb*/, size_t s1/*size*/){
//  
//  void *p_addr;
//  
//  rt_mutex_take(__g_malloc_mutex, RT_WAITING_FOREVER);
//  p_addr = calloc(s0, s1);
//  rt_mutex_release(__g_malloc_mutex);
//}



//int _mutex_initialize(rt_mutex_t sid)
//{
//   /* Create a mutex semaphore */
//    rt_mutex_t tmpmutex;
//    tmpmutex = rt_mutex_create("stdlib", RT_IPC_FLAG_FIFO);
//    if (tmpmutex == RT_NULL)
//       while(1);
//    else
//    {
//        rt_mutex_release(tmpmutex);
//        *((uint32_t *)sid) = (uint32_t)tmpmutex;
//    }
//    return 1;
//}


//void _mutex_acquire(uint32_t sid)
//{
//    /* Task sleep until get semaphore */
//	  rt_mutex_t tmpmutex;
//    tmpmutex = (rt_mutex_t)sid;
//    rt_mutex_take(tmpmutex, RT_WAITING_FOREVER);

//    return;
//}


//void _mutex_release(uint32_t sid)
//{
//    /* Release the semaphore. */
//		rt_mutex_t tmpmutex;
//    tmpmutex = (rt_mutex_t)sid;
//    rt_mutex_release(tmpmutex);
//}


//void _mutex_free(uint32_t sid)
//{
//    /* Free the semaphore. */
//  	 rt_mutex_t tmpmutex;
//     tmpmutex = (rt_mutex_t)sid;
//     rt_mutex_delete(tmpmutex);
//}

