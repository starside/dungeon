/*
 * scull.h -- definitions for the char module
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
 * $Id: scull.h,v 1.15 2004/11/04 17:51:18 rubini Exp $
 */

#ifndef _GAME_H_
#define _GAME_H_

#define GAME_NR_DEVS 1

struct game_dev {
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct cdev cdev;	  /* Char device structure		*/
};

int     game_p_init(dev_t dev);
void    game_p_cleanup(void);
ssize_t game_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos);
ssize_t game_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos);


#endif
