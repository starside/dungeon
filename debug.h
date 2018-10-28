#ifndef __GAME_DEBUG__
#define __GAME_DEBUG__
#include <linux/kernel.h>

#define GAME_TRACE() printk(KERN_NOTICE "GAME TRACE: Called %s\n", __func__)
#define DEBUG_WARN(message) printk(KERN_NOTICE "%s\n", message)

#endif
