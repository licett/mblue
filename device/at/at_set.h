/*
 * =====================================================================================
 *
 *       Filename:  at_set.h
 *
 *    Description:  definitions of commands and scanner about AT sets that are 
 *    supported in mblue
 *
 *        Version:  1.0
 *        Created:  12/22/2017 08:53:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  at_set_INC
#define  at_set_INC
#include "at_parser.h"
enum at_set_index {
	AT_SET_MC8618,
	AT_SET_ESP8266,
	AT_SET_XXX,
};

struct at_sets;
typedef enum at_response_type (*at_sets_scanner)(struct at_sets *at_sets, const char *line, size_t len);
/*-----------------------------------------------------------------------------
 *  at sets object.
 *  It contains an AT command set and a urc response scan function
 *-----------------------------------------------------------------------------*/
struct at_sets {
	struct at_set_item *at_set_items;
	at_sets_scanner urc_scan;
	int (*recv_scan)(struct at_sets *at_sets, const char *line, size_t len);
};
struct at_set_item {
	const char *cmd;
	at_sets_scanner scanner;
};
struct at_sets *mblue_at_sets_get(uint16_t index);
extern const char *mc8618_urc_responses[];
#endif   /* ----- #ifndef at_set_INC  ----- */
