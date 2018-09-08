/*
 * =====================================================================================
 *
 *       Filename:  mblue_map.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/16/2017 04:41:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  mblue_map_INC
#define  mblue_map_INC
#include "mblue_stddefine.h"

#define	GUARD_BYTES_LEN			1024				/* 1024 is big enough to store a data fragment  */
#define	GUARD_VECTOR_LEN		32				/* 128 is big enough */
#define	GUARD_MAGIC			0x45454545			/* detect OUTOFBOUND */

#define PAYLOAD_P(x)			(((uint32_t) (x) & 3) == 0)
#define INTEGER_P(x)			(((uint32_t) (x) & 3) == 1)
#define POINTER_STATIC(x)		(((uint32_t) (x) & 3) == 2)
#define POINTER_DYNAMIC(x)		(((uint32_t) (x) & 3) == 3)
#define POINTER_P(x)			(POINTER_STATIC (x) || POINTER_DYNAMIC (x))

#define GET_INTEGER(x)			((uint32_t) (x) >> 2)
#define MAKE_INTEGER(x)			((struct mblue_payload *) (((x) << 2) | 1))

#define GET_POINTER(x)			((uint32_t) (x) & 0xFFFFFFFC)
#define MAKE_POINTER(x)			((struct mblue_payload *) ((uint32_t)(x) | 2))

typedef enum mblue_payload_type { pair, byte, vector} PAYLOAD_TYPE;
struct mblue_payload {
	PAYLOAD_TYPE type;
	union {
		struct { struct mblue_payload *car, *cdr; } pv_pair;
		struct { uint32_t length; uint8_t *elts; } pv_byte;
		struct { uint32_t length; struct mblue_payload **elts; } pv_vector;
	} value;
};

struct mblue_payload_vector_iter {
	struct mblue_payload *mp;
	struct mblue_payload **pos;
};

struct mblue_payload_ops {
	// allocate a payload struct that contains a byte bundler
	struct mblue_payload *(*alloc_byte)(
		struct mblue_byte_bundler *mb);
	// allocate a vector struct with fix number items
	struct mblue_payload *(*alloc_vector)(uint32_t size);

	int (*release_vector)(struct mblue_payload *v_vector);
	int (*release_byte)(struct mblue_payload *v_byte);

	// default parse entry 
	int (*parse)(struct mblue_payload *payload, 
		void *dest_struct);
	// add a item to vector and the position is determined by iterator
	void (*put)(struct mblue_payload_vector_iter *iter, 
		struct mblue_payload *value);
	// init a iterator 
	int (*iter_init)(struct mblue_payload_vector_iter *iter, 
		struct mblue_payload *payload);
	// iter to next
	int (*iter_next)(struct mblue_payload_vector_iter *iter);
};

#endif   /* ----- #ifndef mblue_map_INC  ----- */
