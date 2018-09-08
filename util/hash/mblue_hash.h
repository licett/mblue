/*
 * =====================================================================================
 *
 *       Filename:  mblue_hash.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/10/8 10:45:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  mblue_hash_INC
#define  mblue_hash_INC

uint32_t sdbm_hash(char *str);
uint32_t sdbm_mem(uint8_t *str, size_t len);
#endif   /* ----- #ifndef mblue_hash_INC  ----- */
