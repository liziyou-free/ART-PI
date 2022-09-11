
///*
//		实现C库的互斥锁接口，以便部分C库函数支持线程安全和可重入

//*/

//#include "rtthread.h"


//extern char  rtthread_is_init;


//int _mutex_initialize(rt_mutex_t sid)
//{
//	if(! rtthread_is_init)
//	{
//		return 0;
//	}
//   /* Create a mutex semaphore */
//    rt_mutex_t tmpmutex;
//    tmpmutex = rt_mutex_create("stdlib", RT_IPC_FLAG_FIFO);
//    if (tmpmutex == RT_NULL)
//        return 0;
//    else
//    {
//        sid = tmpmutex;
//    }
//    return 1;
//}


//void _mutex_acquire(rt_mutex_t sid)
//{
//		if(! rtthread_is_init)
//		{
//			return ;
//		}
//	
//    /* Task sleep until get semaphore */
//	
//    rt_mutex_take(sid, RT_WAITING_FOREVER);

//    return;
//}


//void _mutex_release(rt_mutex_t sid)
//{
//	if(! rtthread_is_init)
//	{
//		return ;
//	}
//    /* Release the semaphore. */
//	
//    rt_mutex_release(sid);
//}


//void _mutex_free(rt_mutex_t sid)
//{
//		if(! rtthread_is_init)
//		{
//			return ;
//		}
//    /* Free the semaphore. */
//     rt_mutex_delete(sid);
//}

