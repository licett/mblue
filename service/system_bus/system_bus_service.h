/*
 * =====================================================================================
 *
 *       Filename:  system_bus_service.h
 *
 *    Description:  system bus service is the core module of mblue, 
 *			acting as a arbitrator to dispatch messages to different tasks
 *
 *        Version:  1.0
 *        Created:  2017/10/24 10:10:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  system_bus_service_INC
#define  system_bus_service_INC
struct system_bus_service {
	struct mblue_segment ms_entity;
	struct system_bus *sb;
};
struct system_bus_service *get_system_bus_service_instance(void);
#endif   /* ----- #ifndef system_bus_service_INC  ----- */
