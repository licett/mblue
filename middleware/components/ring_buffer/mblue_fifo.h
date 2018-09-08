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
#ifndef _MBLUE_FIFO_H
#define _MBLUE_FIFO_H
#include "mblue_stddefine.h"

#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

struct mblue_fifo {
	unsigned char *buffer;	/* the buffer holding the data */
	unsigned int size;	/* the size of the allocated buffer */
	unsigned int in;	/* data is added at offset (in % size) */
	unsigned int out;	/* data is extracted from off. (out % size) */
	//void *lock;		/* lock to support mpmc */

	int (*init)(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int size);
	unsigned int (*put)(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int len);		
	unsigned int (*get)(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int len);		
	unsigned int (*len)(struct mblue_fifo *fifo);
	void (*reset)(struct mblue_fifo *fifo);
/*
	unsigned int (*sync_put)(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int len);		
	unsigned int (*sync_get)(struct mblue_fifo *fifo, unsigned char *buffer, unsigned int len);		
	void (*sync_reset)(struct mblue_fifo *fifo);
*/
};

struct mblue_fifo *mblue_fifo_alloc(unsigned int size);
void mblue_fifo_free(struct mblue_fifo *fifo);
#endif
