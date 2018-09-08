/*
 * =====================================================================================
 *
 *       Filename:  pb_schema.c
 *
 *    Description:  this file defines all of pb data layouts used in mblue
 *
 *        Version:  1.0
 *        Created:  12/15/2017 10:06:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mblue.pb.h"
#include "pb_schema.h"
#include "pb_encode.h"
#include "pb_decode.h"

#include "mblue_heap.h"
#include "mblue_assert.h"

static bool drain_input_buffer(pb_istream_t *stream, size_t count)
{
	pb_byte_t tmp[16];
	while (count > 16)
	{
		if (!pb_read(stream, tmp, 16))
			return false;

		count -= 16;
	}

	return pb_read(stream, tmp, count);

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mblue_serialize
 *  Description:  alloc a buffer and serialize dest struct to this buffer
 *  the data layout is layout_index+len+content
 *  @param index : data layout index of struct, here we use a protocal buffer filed type
 *  @param src_struct  : struct that will be serialized
 *  @return  
 	valid buffer : success 
	NULL : failed
 * =====================================================================================
 */
void *mblue_serialize_rpc(const char *method, uint16_t index, void *src)
{
	int res;
	void *buffer;
	pb_ostream_t stream;
	struct _dl_rpc_header header;
	uint32_t header_size, data_size, buffer_size;
	const pb_field_t *data_schema;

	_ASSERT(method != NULL);
	_ASSERT(strlen(method) <= MAX_RPC_METHOD_NAME_LENGTH);
	_ASSERT(index < PB_MAX);

	// 1.check
	res = -1;
	buffer = NULL;
	data_size = 0;
	header_size = 0;
	data_schema = mblue_pb_data_layout_get(index);
	strncpy(header.method_name, method, MAX_RPC_METHOD_NAME_LENGTH);
	header.layout_index = index;
	
	// 2.get buffer size
	if (!pb_get_encoded_size((size_t *)&header_size, dl_rpc_header_fields, &header)) {
		goto exited;
	}
	if (data_schema && 
		!pb_get_encoded_size((size_t *)&data_size, data_schema, src)) {
		goto exited;
	}

	// 3.allocate buffer(more space to store len)
	buffer_size = header_size + 4 +
				(data_size ? data_size + 4 : 0);
	buffer = (uint8_t *)mblue_malloc(buffer_size); 
	if (!buffer) {
		goto exited;
	}

	// 4.serialize the struct to buffer
	stream = pb_ostream_from_buffer(buffer, buffer_size);	
	if (!pb_encode_delimited(&stream, dl_rpc_header_fields, &header)) {
		goto exited;
	}
	if (data_schema &&
		!pb_encode_submessage(&stream, data_schema, src)) {
		mblue_free(buffer);
		goto exited;
	}

	// 5.return buffer to caller
	_ASSERT(data_size + header_size < stream.bytes_written);
	res = 0;

exited:
	if (res && buffer) {
		mblue_free(buffer);
		buffer = NULL;
	}
	return buffer;
}

void *mblue_serialize_struct_with_layout(void *src, const pb_field_t fields[])
{
	void *p;
	size_t size;
	pb_ostream_t stream;

	p = NULL;
	if (!pb_get_encoded_size(&size, fields, src)) {
		goto exited;
	}
	p = mblue_malloc(size + 4);
	if (!p) {
		goto exited;
	}

	stream = pb_ostream_from_buffer(p, size + 4);
	if (!pb_encode_delimited(&stream, fields, src)) {
		mblue_free(p);
		p = NULL;
	}
exited:
	return p;
}

mblue_errcode mblue_unserialize_struct_with_layout(
	const void *data, const pb_field_t fields[], void *dest_struct)
{
	pb_istream_t stream;
	
	stream = pb_istream_from_buffer(data, SIZE_MAX);
	if (!pb_decode_delimited(&stream, fields, dest_struct)) {
		return MBLUE_ERR_PB_DECODE;
	}
	return MBLUE_OK;
}

void *mblue_serialize_mono_buffer(const void *buffer, size_t len)
{
	struct mblue_byte_bundler bundler = {len, (void *)buffer};
	struct _dl_array da = {
		{
			{.encode = __mblue_pb_write_bytes}, 
			(void *)&bundler
		}
	};

	return mblue_serialize_struct_with_layout(&da, dl_array_fields);
}

mblue_errcode mblue_unserialize_mono_buffer(
	const void *buffer, struct mblue_byte_bundler *bundler)
{
	mblue_errcode rc;
	struct _dl_array da = {{{.decode = __mblue_pb_get_bytes_pointer}, (void *)bundler}};

	/*rc = MBLUE_ERR_PB_DECODE;
	stream = pb_istream_from_buffer(buffer, SIZE_MAX);
	if (!pb_decode_delimited(&stream, dl_array_fields, &da)) {
		goto exited;
	}
	rc = MBLUE_OK;*/
	rc = mblue_unserialize_struct_with_layout(buffer, dl_array_fields, &da);
	return rc;
}

bool __mblue_pb_get_bytes_pointer(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
	struct mblue_byte_bundler *bb;
	
	bb = *arg;
	bb->byte = stream->state;
	bb->len = stream->bytes_left;

	/* Skip input bytes */
	return drain_input_buffer(stream, stream->bytes_left);
}

bool __mblue_pb_read_bytes(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
	size_t len;
	uint8_t *p;
	struct mblue_byte_bundler *bb;

	len = stream->bytes_left; 
	p = mblue_malloc(len + 1);
	if (!p) {
		return false;
	}
	if (!pb_read(stream, p, len)) {
		mblue_free(p);
		p = NULL;
	}
	bb = *arg;
	bb->byte = p;
	bb->len = len;

	return true;
}

bool __mblue_pb_write_bytes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
	struct mblue_byte_bundler *bundler;

	bundler = (struct mblue_byte_bundler *)*arg;
	return pb_encode_tag_for_field(stream, field) &&
		pb_encode_string(stream, bundler->byte, bundler->len);
}

static SMART_DATA dl12_parser(pb_istream_t *stream, void *f)
{
	mblue_errcode rc;
	INIT_PB_DECODE_DATA_STRUCT(dl_12, NULL, 0);
	SMART_DATA (*func)(int, uint8_t *, size_t);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_12_fields, &dl_12_var)) {
		return MAKE_INTEGER(rc);
	}

	func = (SMART_DATA (*)(int, uint8_t *, size_t))f;
	return func(dl_12_var.num, (uint8_t *)tmp_bundle.byte, tmp_bundle.len);
}

#define	EXTEND_PARSER(data_type, func_type, func, has_data, ret)			\
	data_type data_type##_var;							\
	int bytes;									\
	mblue_errcode rc;								\
	struct mblue_byte_bundler bundler = {0, NULL};					\
	func_type func_type##func;							\
	rc = MBLUE_ERR_PB_DECODE;							\
	if (has_data) {									\
		ar.data.funcs.decode = __mblue_pb_read_bytes;				\
		ar.data.arg = &bundler;							\
	}										\
	if (!pb_decode_delimited(stream, data_type##_fields, &data_type##_var)) {	\
		goto exited;								\
	}										\
	func_type##func = (func_type)func;						\
	ret = func(&data_type##_var)							\


static SMART_DATA simple_request_parser(pb_istream_t *stream, void *f)
{
	return ((SMART_DATA (*)(void))f)();
}

static SMART_DATA dl112_parser(pb_istream_t *stream, void *f)
{
	mblue_errcode rc;
	INIT_PB_DECODE_DATA_STRUCT(dl_112,  NULL, 0);
	SMART_DATA (*func)(int, int, uint8_t *, size_t);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_112_fields, &dl_112_var)) {
		return MAKE_INTEGER(rc);
	}

	func = (SMART_DATA (*)(int, int, uint8_t *, size_t))f;
	return func(dl_112_var.num1, dl_112_var.num2, tmp_bundle.byte, tmp_bundle.len);
}

static SMART_DATA dl113_parser(pb_istream_t *stream, void *f)
{
	mblue_errcode rc;
	INIT_PB_DECODE_DATA_STRUCT(dl_113,  NULL, 0);
	SMART_DATA (*func)(int , int, char *);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_113_fields, &dl_113_var)) {
		return MAKE_INTEGER(rc);
	}
	func = (SMART_DATA (*)(int , int, char *))f;
	return func(dl_113_var.num1, dl_113_var.num2, (char *)tmp_bundle.byte);
}

static SMART_DATA dl111_parser(pb_istream_t *stream, void *f)
{
	dl_111 var;
	mblue_errcode rc;
	SMART_DATA (*func)(int , int, int);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_111_fields, &var)) {
		return MAKE_INTEGER(rc);
	}
	rc = MBLUE_REMOTE_CALL_PENDING;
	func = (SMART_DATA (*)(int , int, int))f;
	return func(var.num1, var.num2, var.num3);
}

static SMART_DATA dl1_parser(pb_istream_t *stream, void *f)
{
	dl_1 var;
	mblue_errcode rc;
	SMART_DATA (*func)(int );

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_1_fields, &var)) {
		return MAKE_INTEGER(rc);
	}
	func = (SMART_DATA (*)(int))f;
	return func(var.num);
}

static SMART_DATA dl1111_parser(pb_istream_t *stream, void *f)
{
	dl_1111 var;
	mblue_errcode rc;
	SMART_DATA (*func)(int , int , int , int);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_1111_fields, &var)) {
		return MAKE_INTEGER(rc);
	}
	func = (void* (*)(int , int , int , int))f;
	return func(var.num1, var.num2, var.num3, var.num4);
}

static SMART_DATA dl1112_parser(pb_istream_t *stream, void *f)
{
	mblue_errcode rc;
	INIT_PB_DECODE_DATA_STRUCT(dl_1112,  NULL, 0);
	SMART_DATA (*func)(int , int , int , uint8_t *, size_t);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_1112_fields, &dl_1112_var)) {
		return MAKE_INTEGER(rc);
	}
	func = (SMART_DATA (*)(int , int , int , uint8_t *, size_t))f;
	return func(dl_1112_var.num1, dl_1112_var.num2, dl_1112_var.num3, tmp_bundle.byte, tmp_bundle.len);
}

static SMART_DATA dl_array_parser(pb_istream_t *stream, void *f)
{
	mblue_errcode rc;
	INIT_PB_DECODE_DATA_STRUCT(dl_array,  NULL, 0);
	SMART_DATA (*func)(uint8_t *, size_t);

	rc = MBLUE_ERR_PB_DECODE;
	if (!pb_decode_delimited(stream, dl_array_fields, &dl_array_var)) {
		return MAKE_INTEGER(rc);
	}
	func = (SMART_DATA (*)(uint8_t *, size_t))f;
	return func(tmp_bundle.byte, tmp_bundle.len);
}

static struct pb_layout_parser mblue_pb_data_layout_array[] = {
	[PB_0]		= {NULL, simple_request_parser},
	[PB_1]		= {dl_1_fields, dl1_parser},
	[PB_2]		= {dl_array_fields, dl_array_parser},
	[PB_12]		= {dl_12_fields, dl12_parser},
	[PB_111]	= {dl_111_fields, dl111_parser},
	[PB_112]	= {dl_112_fields, dl112_parser},
	[PB_113]	= {dl_113_fields, dl113_parser},
	[PB_1111]	= {dl_1111_fields, dl1111_parser},
	[PB_1112]	= {dl_1112_fields, dl1112_parser},
};

const pb_field_t *mblue_pb_data_layout_get(uint16_t index)
{
	return mblue_pb_data_layout_array[index].field;
}

mblue_pb_parser_func_t mblue_pb_data_parser_get(uint16_t index)
{
	return mblue_pb_data_layout_array[index].parser;
}

