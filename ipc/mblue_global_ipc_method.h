/*
 * =====================================================================================
 *
 *       Filename:  global_rpc_method.h
 *
 *    Description:  declare of all global rpc method
 *
 *        Version:  1.0
 *        Created:  12/23/2017 01:15:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  global_rpc_method_INC
#define  global_rpc_method_INC
#include "mblue_stddefine.h"

mblue_errcode log_service_set_type(uint32_t type);
mblue_errcode log_service_set_level(uint32_t level);
mblue_errcode log_service_write(uint32_t level, char *source, void *fmt, ...);
mblue_errcode log_service_hex(const char *prefix, const char *buffer, size_t len);
#endif   /* ----- #ifndef global_rpc_method_INC  ----- */
