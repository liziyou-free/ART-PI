/**
 * @file lv_fs_if.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_if.h"


/*********************
 *      DEFINES
 *********************/
#define LV_FS_IF_FATFS   ('S')
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

void lv_fs_if_fatfs_init(void);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/**
 * Register driver(s) for the File system interface
 */
void lv_fs_if_init(void)
{
	lv_fs_if_fatfs_init();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

