/*
 * A simple kernel FIFO implementation.
 *
 * Copyright (C) 2004 Stelian Pop <stelian@popies.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <string.h>
#include "mblue_fifo.h"
#include "mblue_assert.h"
#include "mblue_heap.h"

/**
 * kfifo_init - init a FIFO object using a preallocated buffer
 * @buffer: the preallocated buffer to be used.
 * @size: the size of the internal buffer, this have to be a power of 2.
 *
 */
static int __kfifo_init(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int size)
{
	_ASSERT(fifo);

	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return 0;
}

/*
 * __kfifo_reset - removes the entire FIFO contents, no locking version
 * @fifo: the fifo to be emptied.
 */
static inline void __kfifo_reset(struct mblue_fifo *fifo)
{
	fifo->in = fifo->out = 0;
}

/**
 * __kfifo_len - returns the number of bytes available in the FIFO, no locking version
 * @fifo: the fifo to be used.
 */
static inline unsigned int __kfifo_len(struct mblue_fifo *fifo)
{
	return fifo->in - fifo->out;
}

/**
 * __kfifo_put - puts some data into the FIFO, no locking version
 * @fifo: the fifo to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
static unsigned int __kfifo_put(struct mblue_fifo *fifo,
			 unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = MIN(len, fifo->size - fifo->in + fifo->out);

	/*
	 * Ensure that we sample the fifo->out index -before- we
	 * start putting bytes into the kfifo.
	 */

	/* first put the data starting from fifo->in to buffer end */
	l = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));
	memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, buffer + l, len - l);

	/*
	 * Ensure that we add the bytes to the kfifo -before-
	 * we update the fifo->in index.
	 */
	fifo->in += len;

	return len;
}

/**
 * __kfifo_get - gets some data from the FIFO, no locking version
 * @fifo: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @buffer and returns the number of copied bytes.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
static unsigned int __kfifo_get(struct mblue_fifo *fifo,
			 unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = MIN(len, fifo->in - fifo->out);

	/*
	 * Ensure that we sample the fifo->in index -before- we
	 * start removing bytes from the kfifo.
	 */

	/* first get the data from fifo->out until the end of the buffer */
	l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
	memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + l, fifo->buffer, len - l);

	/*
	 * Ensure that we remove the bytes from the kfifo -before-
	 * we update the fifo->out index.
	 */
	fifo->out += len;

	return len;
}

/**
 * kfifo_alloc - allocates a new FIFO and its internal buffer
 * @size: the size of the internal buffer to be allocated.
 * @lock: the lock to be used to protect the fifo buffer
 *
 * The size will be rounded-up to a power of 2.
 */
struct mblue_fifo *mblue_fifo_alloc(unsigned int size)
{
	unsigned char *buffer;
	struct mblue_fifo *fifo;

	/*
	 * size must be power of 2, since our 'let the indices
	 * wrap' tachnique works only in this case.
	 */
	_ASSERT(is_power_of_2(size));

	fifo = NULL;
	buffer = (unsigned char *)mblue_malloc(size);
	if (!buffer) {
		goto failed;
	}

	fifo = (struct mblue_fifo *)mblue_malloc(sizeof(struct mblue_fifo));
	if (!fifo) {
		goto failed_obj;
	}

	fifo->init = __kfifo_init;
	fifo->put = __kfifo_put;
	fifo->get = __kfifo_get;
	fifo->len = __kfifo_len;
	fifo->reset = __kfifo_reset;
	fifo->init(fifo, buffer, size);
	return fifo;

failed_obj:
	mblue_free(buffer);
failed:
	return NULL;
}

/**
 * kfifo_free - frees the FIFO
 * @fifo: the fifo to be freed.
 */
void mblue_fifo_free(struct mblue_fifo *fifo)
{
	_ASSERT(fifo);

	mblue_free(fifo->buffer);
	mblue_free(fifo);
}
