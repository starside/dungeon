#include <linux/slab.h>		/* kmalloc() */
#include <linux/circ_buf.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include "io.h"
#include "debug.h"

int init_ringbuffer(struct Player_ringbuffer **buf) {
	GAME_TRACE();
	*buf = kmalloc(sizeof(**buf), GFP_KERNEL);
	if(!*buf) {
		return -1;
	}
	memset(*buf, 0, sizeof(struct Player_ringbuffer));
	MUTEX_INIT( &((*buf)->lock) );
	return 0;
}

int free_ringbuffer(struct Player_ringbuffer *buf){
	GAME_TRACE();
	if( down_interruptible(&(buf->lock)) ) {
		return -1;
	}
	struct semaphore temp;
	memcpy(&temp, &(buf->lock), sizeof(temp) );
	kfree((void *)buf);
	up(&temp);
	return 0;
}

int write_buffer(struct Player_ringbuffer *buf, const char *message, size_t len) {
    GAME_TRACE();
    int writelen = 0;
    if( down_interruptible(&(buf->lock)) ){
        return -ERESTARTSYS;
    }
    
    writelen = min(len, (size_t)CIRC_SPACE(buf->head, buf->tail, PLAYER_IO_BUF_LEN));
    printk(KERN_INFO "writelen: %d, head %d, tail %d\n", writelen, buf->head, buf->tail);
    memcpy((void *)&(buf->buf[buf->head]), (const void *)message, writelen);
    buf->head = (writelen + buf->head) % PLAYER_IO_BUF_LEN;
    up(&(buf->lock));
    return writelen;
}

int read_buffer_to_user(struct Player_ringbuffer *buf, char *dest, size_t len) {
    GAME_TRACE();
    int readlen = 0;
    if( down_interruptible(&(buf->lock)) ){
        return -ERESTARTSYS;
    }
    
    readlen = min(len, (size_t)CIRC_CNT(buf->head, buf->tail, PLAYER_IO_BUF_LEN));
    if( copy_to_user(dest, &(buf->buf[buf->tail]), readlen) ) {
		readlen = -EFAULT;
        goto out;
	}

    buf->tail += readlen;
    buf->tail = buf->tail % PLAYER_IO_BUF_LEN;
out:
    up(&(buf->lock));
    return readlen;
}
