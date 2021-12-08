/*
* Created by Aen on 2021/7/15.
*/
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
//#include <debug_manager.h>

#define FB_DEVICE_NAME "/dev/fb0"

#define COLOR_BACK_GROUND   0xE7DBB5  /* 泛黄的纸 */
#define COLOR_FORE_GROUND   0x514438  /* 褐色字体 */

//#define DBG_PRINTF DebugPrint
#define DBG_PRINTF(...)
//#define DBG_PRINTF printf
#endif //_CONFIG_H
