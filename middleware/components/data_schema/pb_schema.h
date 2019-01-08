/*
 * =====================================================================================
 *
 *       Filename:  pb_schema.h
 *
 *    Description:  this file defines all of pb data layouts used in mblue
 *
 *        Version:  1.0
 *        Created:  12/15/2017 10:08:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  pb_schema_INC
#define  pb_schema_INC
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "mblue.pb.h"
#include "mblue_errcode.h"
#include "mblue_macro.h"

#define	MAX_IPC_METHOD_NAME_LENGTH		31			/*  */

#define	INIT_PB_DECODE_DATA_STRUCT(type, byte_p, byte_n)				\
	INIT_PB_DATA_STRUCT(type, 0, 1, byte_p, byte_n)

#define	INIT_PB_DATA_STRUCT(type, is_encode, has_byte, byte_p, byte_n)			\
	type type##_var;								\
	struct mblue_byte_bundler tmp_bundle = {byte_n, byte_p};			\
	if (has_byte) {									\
		if (is_encode) {							\
			type##_var.data.funcs.encode = __mblue_pb_write_bytes;		\
		} else {								\
			type##_var.data.funcs.decode = __mblue_pb_get_bytes_pointer;	\
		}									\
		type##_var.data.arg = &tmp_bundle;					\
	}

struct mblue_byte_bundler {
	size_t len;
	uint8_t *byte;
};
typedef SMART_DATA (*mblue_pb_parser_func_t)(pb_istream_t *, void *); 
typedef enum {
	PB_0,	// SIMPLE REQUEST 
	PB_1,
	PB_2,
	PB_12,	
	PB_112,
	PB_113,
	PB_111,
	PB_1111,
	PB_1112,
	PB_MAX
}PB_TYPE_INDEX;

struct pb_layout_parser {
	const pb_field_t *field;
	mblue_pb_parser_func_t parser;
};
void *mblue_serialize_struct_with_layout(void *src, const pb_field_t fields[]);
void *mblue_serialize_mono_buffer(const void *buffer, size_t len);
void *mblue_serialize_ipc(const char *method, uint16_t index, void *src);
bool __mblue_pb_write_bytes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
bool __mblue_pb_read_bytes(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool __mblue_pb_get_bytes_pointer(pb_istream_t *stream, const pb_field_t *field, void **arg);
mblue_errcode mblue_unserialize_mono_buffer(const void *buffer, struct mblue_byte_bundler *bundler);
mblue_errcode mblue_unserialize_struct_with_layout(
	const void *data, const pb_field_t fields[], void *dest_struct);
const pb_field_t *mblue_pb_data_layout_get(uint16_t index);
mblue_pb_parser_func_t mblue_pb_data_parser_get(uint16_t index);
#endif   /* ----- #ifndef pb_schema_INC  ----- */
