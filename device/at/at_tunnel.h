/*
 * =====================================================================================
 *
 *       Filename:  at_tunnel.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/21/2017 01:42:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  at_tunnel_INC
#define  at_tunnel_INC
#include "mblue_errcode.h"
#include "mblue_stddefine.h"
#include "at_parser.h"

struct at_tunnel;
typedef size_t (*at_execute_t)(uint16_t com, uint8_t *data, size_t len);
typedef mblue_errcode (*at_urc_t)(struct at_tunnel *at, uint8_t *data, size_t len);
struct at_tunnel_operation {
	at_execute_t execute;
	at_urc_t urc;
};
struct at_tunnel {
	uint16_t com;
	uint16_t at_set_index;
	int32_t cmd_index;
	struct at_parser *parser;
	void *priv;
	struct at_tunnel_operation ops;
};

struct at_tunnel *alloc_at_tunnel(
	uint16_t com, uint16_t at_set_index, void *priv, 
	at_execute_t execute, at_urc_t urc);
#endif   /* ----- #ifndef at_tunnel_INC  ----- */
