/*
 * =====================================================================================
 *
 *       Filename:  mblue_map.c
 *
 *    Description:  
 *    1. This module implements a simple map that supports self-explaining
 *    2. It should be easy to decode the data layout in runtime
 *
 *    TODO:
 *    1. add pair implementation
 *    2. support capacity changing in runtime
 *        Version:  1.0
 *        Created:  12/16/2017 04:40:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_map.h"
#include "mblue_assert.h"
#include "mblue_heap.h"

#define	CHECK_PAYLOAD_VECTOR(mp)			\
	_ASSERT(mp);					\
	_ASSERT(POINTER_P(mp));				\
	_ASSERT(mp->type == vector);			\
	_ASSERT(mp->value.pv_vector.elts);		\
	_ASSERT(mp->value.pv_vector.length > 0);	\
	_ASSERT(mp->value.pv_vector.length < GUARD_VECTOR_LEN)		

#define	CHECK_PAYLOAD_BYTE(mp)				\
	_ASSERT(mp);					\
	_ASSERT(POINTER_P(mp));				\
	_ASSERT(mp->type == byte);			\
	_ASSERT(mp->value.pv_byte.elts);		\
	_ASSERT(mp->value.pv_byte.length > 0);		\
	_ASSERT(mp->value.pv_byte.length < GUARD_BYTES_LEN)		

#define	CHECK_PAYLOAD_ITER(iter)			\
	_ASSERT(iter);					\
	_ASSERT(iter->mp);				\
	_ASSERT(iter->pos)				

static struct mblue_payload *mblue_payload_vector_alloc(uint32_t size)
{
	struct mblue_payload *p;
	uint32_t *q;

	// 1.allocate main vector 
	p = (struct mblue_payload *)mblue_malloc(
		sizeof(struct mblue_payload));
	_ASSERT(p);
	if (!p) {
		goto failed_vector;
	}

	// 2.allocate entries(1 more entry to guard OUTOFBOUND error)
	q = (uint32_t *)mblue_malloc((size + 1) * sizeof(void *));
	if (!q) {
		goto failed_entries;
	}
	q += size;
	*q = GUARD_MAGIC;

	// 3.fill the vector
	p->type = vector;
	p->value.pv_vector.length = size;
	p->value.pv_vector.elts = (struct mblue_payload **)q;
	return p;

failed_entries:
	mblue_free(p);
	p = NULL;
failed_vector:
	return p;
}

static struct mblue_payload *mblue_payload_byte_alloc(struct mblue_byte_bundler *mb)
{
	struct mblue_payload *mp;

	mp = mblue_malloc(sizeof(struct mblue_payload));
	if (mp) {
		mp->type = byte;
		mp->value.pv_byte.length = mb->len;
		mp->value.pv_byte.elts = mb->byte;
	}
	return mp;
}

static int mblue_payload_byte_release(struct mblue_payload *bytes)
{
	CHECK_PAYLOAD_BYTE(bytes);
	
	// user should free data by themselves
	//mblue_free(bytes->value.pv_byte.elts);
	mblue_free(bytes);
	return 0;
}

static int mblue_payload_vector_iter_init(
	struct mblue_payload_vector_iter *iter, struct mblue_payload *mp)
{
	CHECK_PAYLOAD_VECTOR(mp);
	_ASSERT(iter);

	iter->mp = mp;
	iter->pos = mp->value.pv_vector.elts;
	return 0;
}

static int mblue_payload_vector_iter_next(struct mblue_payload_vector_iter *iter)
{
	CHECK_PAYLOAD_ITER(iter);
	iter->pos++;
	if (iter->pos - iter->mp->value.pv_vector.elts < iter->mp->value.pv_vector.length
		&& *((uint32_t *)(iter->pos)) != GUARD_MAGIC) {
		return 0;
	}
	return -1;
}

static void mblue_payload_vector_put(struct mblue_payload_vector_iter *iter, struct mblue_payload *value)
{
	CHECK_PAYLOAD_ITER(iter);
	
	*(iter->pos) = value;
}

static int mblue_payload_vector_release(struct mblue_payload *pv_vector)
{
	struct mblue_payload **mp;
	struct mblue_payload *p;

	CHECK_PAYLOAD_VECTOR(pv_vector);
	mp = pv_vector->value.pv_vector.elts;

	// release items
	for(uint32_t i = 0; i < pv_vector->value.pv_vector.length; i ++) {
		p = *mp;
		if (PAYLOAD_P(p)) {
			switch(p->type) {
			// 1.byte type
			case byte:
				CHECK_PAYLOAD_BYTE(p);
				mblue_payload_byte_release(p);
				break;

			// 2.vector type
			case vector:
				// embedded vector is not recommanded and 
				// note that this will consume more stack space
				mblue_payload_vector_release(p);
				break;

			default:
				_ASSERT(FALSE);
				break;
			}
		}
		mp++;
	}

	// release entry
	mblue_free(pv_vector->value.pv_vector.elts);
	mblue_free(pv_vector);

	return 0;
}

static int mblue_payload_vector_get(struct mblue_payload_vector_iter *iter, struct mblue_payload **value)
{
	CHECK_PAYLOAD_ITER(iter);
	_ASSERT(value);
	_ASSERT(*(iter->pos));

	if (!value || !*(iter->pos)) {
		return -1;
	}
	*value = *(iter->pos);
	return 0;
}

static void mblue_payload_parse_byte(struct mblue_payload *pv_byte, struct mblue_byte_bundler *dest)
{
	CHECK_PAYLOAD_BYTE(pv_byte);
	
	dest->byte = pv_byte->value.pv_byte.elts;
	dest->len = pv_byte->value.pv_byte.length;
}

static int mblue_payload_parse_vector(struct mblue_payload *pv_vector, void *dest_struct)
{
	int ret;
	uint32_t *p;
	struct mblue_payload *q;
	struct mblue_byte_bundler *bb;
	struct mblue_payload_vector_iter iter;

	CHECK_PAYLOAD_VECTOR(pv_vector);
	ret = 0;
	p = (uint32_t *)dest_struct;
	mblue_payload_vector_iter_init(&iter, pv_vector);

	do {
		if (*p == GUARD_MAGIC) {
			_ASSERT(FALSE);
			LOGGER("premature end in parsing vector\r\n");
			goto exited;
		}
		ret = mblue_payload_vector_get(&iter, &q);
		if (ret) {
			return -1;
		}
		if (INTEGER_P(q) || POINTER_P(q)) {
			if (INTEGER_P(q))  {
				*p = GET_INTEGER(q);
			} else {
				*p = GET_POINTER(q);	
			}
			p++;
			continue;
		} 
		switch(q->type) {
		case byte:
			bb = (struct mblue_byte_bundler *)p;
			mblue_payload_parse_byte(q, bb);
			p += sizeof(struct mblue_byte_bundler) / sizeof(uint32_t);
			break;
		case pair:
			_ASSERT(FALSE);
			break;
		default:
			break;
		}
	} while(!mblue_payload_vector_iter_next(&iter));

exited:
	return ret;
}

static int mblue_payload_parse(struct mblue_payload *payload, void *dest_struct)
{
	int ret;
	uint32_t *p;
	struct mblue_payload *q;
	struct mblue_byte_bundler *bb;

	_ASSERT(payload);
	_ASSERT(dest_struct);
	p = dest_struct;
	q = payload;
	ret = 0;

	if (INTEGER_P(q)) {
		*p = GET_INTEGER(q);
		goto exited;
	} 
	switch(q->type) {
	case byte:
		bb = (struct mblue_byte_bundler *)p;
		mblue_payload_parse_byte(q, bb);
		break;
	case vector:
		ret = mblue_payload_parse_vector(q, dest_struct);
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	_ASSERT(!ret);
exited:
	return ret;
}

struct mblue_payload_ops payload_operation = {
	.alloc_byte	= mblue_payload_byte_alloc,
	.alloc_vector	= mblue_payload_vector_alloc,
	.release_byte	= mblue_payload_byte_release,
	.release_vector = mblue_payload_vector_release,
	.parse		= mblue_payload_parse,
	.put		= mblue_payload_vector_put,
	.iter_init	= mblue_payload_vector_iter_init,
	.iter_next	= mblue_payload_vector_iter_next,
};
