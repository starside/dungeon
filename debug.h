#ifndef __GAME_DEBUG__
#define __GAME_DEBUG__
#include <linux/kernel.h>

#define GAME_TRACE() printk(KERN_NOTICE "GAME TRACE: Called %s\n", __func__)

#endif
