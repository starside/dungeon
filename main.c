/*
 * main.c -- the bare game char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <asm/semaphore.h>

#include "game.h"		/* local definitions */
#include "messages.h"		/* Hard coded game messages */
#include "entity.h"

struct Player_ringbuffer {
	int head;
	int tail;
	char buf[512];
	struct semaphore lock;
};

struct Player_IOBuffer {
	struct Player_ringbuffer output;
	struct Player_ringbuffer input;
};


/*
 * Players in the game
 */
#define NUM_PLAYERS 2
struct Player game_players[NUM_PLAYERS] = {
					{"Josh", -1},
					{"Natalie", -1}
};

/*
 * Players are assigned based on a stack
 * player_sp takes an index to the nearest free player
 */
unsigned int player_sp = 0;
DECLARE_MUTEX(playerstack_mutex);


/*
 * Our parameters which can be set at load time.
 */

int game_major, game_minor;		/* device numbers */
int game_nr_devs = GAME_NR_DEVS;	/* number of bare game devices */
int cur = 0;

MODULE_AUTHOR("Joshua Kelly");
MODULE_LICENSE("Dual BSD/GPL");

struct game_dev *game_devices;	/* allocated in game_init_module */

/*
 * Open and close
 */

int game_open(struct inode *inode, struct file *filp)
{
	int result = 0;
	cur = 0;
	filp->private_data = NULL;
	if(down_interruptible(&playerstack_mutex)) // Lock player stack
		return -ERESTARTSYS;
	if(player_sp >= NUM_PLAYERS) {
		result = -EINTR;
		printk(KERN_WARNING "Too Many Players");
	} else {
		filp->private_data = (void *)&game_players[player_sp];
		player_sp++;
	}	

	up(&playerstack_mutex);
	return result;          /* success */
}

int game_release(struct inode *inode, struct file *filp)
{
	if(!filp->private_data) {
		return 0;
	}
	if(down_interruptible(&playerstack_mutex)){ // Lock player stack
		return -ERESTARTSYS;
	} else {
		player_sp--;
	}	

out:
	up(&playerstack_mutex);
	return 0;
}

/*
 * Data management: read and write
 */

ssize_t game_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
	char *message = welcome;
	int message_len = sizeof(welcome);
	int result = 0;
        printk(KERN_WARNING "Name: %s\n", ((struct Player *)filp->private_data)->name);	
	if(cur >= message_len) {
		result = 0;
		goto out;
	}
		
	int write_len = min(count, (size_t)(message_len-cur)); // Integer conversion error
		
	if( copy_to_user(buf, &(message[cur]), write_len) ) {
		goto fail;
	}
        cur += write_len;
	result = write_len;

	out:
	return result;
	
	fail:
	return -EFAULT;
}

ssize_t game_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	return 0;	
}

struct file_operations game_fops = {
	.owner =    THIS_MODULE,
	.read =     game_read,
	.write =    game_write,
	.open =     game_open,
	.release =  game_release,
};

/*
 * Finally, the module stuff
 */

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void game_cleanup_module(void)
{
	int i;
	dev_t devno = MKDEV(game_major, 0);

	/* Get rid of our char dev entries */
	if (game_devices) {
		for (i = 0; i < game_nr_devs; i++) {
			//game_trim(game_devices + i);
			cdev_del(&game_devices[i].cdev);
		}
		kfree(game_devices);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, game_nr_devs);
}


/*
 * Set up the char_dev structure for this device.
 */
static void game_setup_cdev(struct game_dev *dev, int index)
{
	int err, devno = MKDEV(game_major, index);
    
	cdev_init(&dev->cdev, &game_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &game_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding game%d", err, index);
}


int game_init_module(void)
{
	int result, i;
	dev_t dev = 0;

/*
 * Get a range of minor numbers to work with, asking for a dynamic
 * major unless directed otherwise at load time.
 */
	result = alloc_chrdev_region(&dev, 0, game_nr_devs,"game");
	game_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "game: can't get major %d\n", game_major);
		return result;
	}

	game_devices = kmalloc(game_nr_devs * sizeof(struct game_dev), GFP_KERNEL);
	if (!game_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(game_devices, 0, game_nr_devs * sizeof(struct game_dev));

        /* Initialize each device. */
	for (i = 0; i < game_nr_devs; i++) {
		game_setup_cdev(&game_devices[i], i);
	}

	return 0; /* succeed */

  fail:
	game_cleanup_module();
	return result;
}

module_init(game_init_module);
module_exit(game_cleanup_module);

