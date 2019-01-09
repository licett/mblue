/*
 * =====================================================================================
 *
 *       Filename:  macro.h
 *
 *    Description:  top macro
 *
 *        Version:  1.0
 *        Created:  2017/4/15 10:34:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  MACRO_INC
#define  MACRO_INC
#define	INLINE	inline							/*  */
#define	CONCAT2(a,b)		a##b				/*  */
#define	CONCAT2E(a,b)		CONCAT2(a,b)			/*  */
#define alignment_up(a, size)   ((a+size-1) & (~ (size-1)))
#define MBLUE_ALIGN(a)		alignment_up(a, 16)   

#define	REGISTER_TASK_EXT(task_name)	task_name##_register()		        /*  */
#define	REGISTER_TASK(task_name)	REGISTER_TASK_EXT(task_name)		/*  */

#define	MODULE_INIT(fn, id)		\
	global_init_module[id] =  fn

#define	MINOR_FUNCTION(minor)		CONCAT2E(TASK_NAME, minor)			/*  */
#define	____STRING(name)			#name					/*  */
#define	STRING(name)			____STRING(name)				/*  */

#define	TASK_STRING_NAME		STRING(TASK_NAME)			/*  */
#define	TASK_ID				CONCAT2E(TASK_NAME, _id)		/*  */
#define	TASK_PRIORITY			CONCAT2E(TASK_NAME, _priority)		/*  */
#define	TASK_STACK_SIZE			CONCAT2E(TASK_NAME, _stack_size)	/*  */
#define	TASK_STACK			CONCAT2E(TASK_NAME, _stack)	        /*  */
#define	TASK_MAX_MSG_IN_QUEUE		CONCAT2E(TASK_NAME, _max_msg_in_queue)	/*  */
#define	TASK_PROC			CONCAT2E(TASK_NAME, _task_proc)		/*  */
#define	TASK_INIT			CONCAT2E(TASK_NAME, _init)		/*  */		
#define	TASK_START			CONCAT2E(TASK_NAME, _start)		/*  */	
#define	TASK_MSG_PROC			CONCAT2E(TASK_NAME, _msg_proc)		/*  */
#define	TASK_MSG_POST			CONCAT2E(TASK_NAME, _msg_post)		/*  */
#define	TASK_REGISTER			CONCAT2E(TASK_NAME, _register)		/*  */
#define	TASK_CALL_SEMAPHORE		CONCAT2E(TASK_NAME, _call_sem)
#define	TASK_MSG_SEMAPHORE		CONCAT2E(TASK_NAME, _msg_sem)
#define	TASK_HANDLE			CONCAT2E(TASK_NAME, _handle)		/*  */
#define	TASK_OBJ			CONCAT2E(TASK_NAME, _obj)		/*  */
#define TASK_PEND                       CONCAT2E(TASK_NAME, _pend)
#define TASK_POST                       CONCAT2E(TASK_NAME, _post)

#define	GET_SEGMENT_FROM_SCENARIO_INSTANCE(name)	\
	(struct mblue_segment *)_SCENARIO_GET_INSTANCE(name)			
#define	GET_SEGMENT_FROM_SERVICE_INSTANCE(name)		\
	(struct mblue_segment *)_SERVICE_GET_INSTANCE(name)			
#define	_SCENARIO_GET_INSTANCE(name)			\
	_SEGMENT_GET_INSTANCE(scenario, name)
#define	_SERVICE_GET_INSTANCE(name)			\
	_SEGMENT_GET_INSTANCE(service, name)
#define	_SEGMENT_GET_INSTANCE(type, name)		\
	type##_##name##_get_instance()

#define	DECLARE_SCENARIO_INCLUDE(name)			\
	_DECLARE_SCENARIO_INCLUDE(name)			
#define	DECLARE_SERVICE_INCLUDE(name)			\
	_DECLARE_SERVICE_INCLUDE(name)			
#define	_DECLARE_SCENARIO_INCLUDE(name)			\
	_DECLARE_SEGMENT_INCLUDE(scenario, name)
#define	_DECLARE_SERVICE_INCLUDE(name)			\
	_DECLARE_SEGMENT_INCLUDE(service, name)
#define	_DECLARE_SEGMENT_INCLUDE(type, name)		\
struct name *type##_##name##_get_instance(void)

#define	DECLARE_SINGLETON_SCENARIO(name)		\
	_DECLARE_SINGLETON_SEGMENT(scenario, name)			
#define	DECLARE_SINGLETON_SERVICE(name)			\
	_DECLARE_SINGLETON_SEGMENT(service, name)			

#define	_DECLARE_SINGLETON_SEGMENT(type, name)		\
static struct name name##_entity;			\
static struct name *name##_instance = NULL;		\
struct name *type##_##name##_get_instance(void)		\
{							\
	if (name##_instance == NULL) {			\
		name##_instance =			\
			&name##_entity;			\
		type##_init(name##_instance);		\
	}						\
	return name##_instance;				\
}

/* smart pointer macros */
typedef	void * SMART_DATA;
#define PB_PAYLOAD(x)			(x && (((size_t) (x) & 3) == 0))
#define INTEGER_P(x)			(((size_t) (x) & 3) == 1)
#define POINTER_S(x)			(GET_POINTER(x) && (((size_t) (x) & 3) == 2))	//pointer to static memory
#define POINTER_H(x)			(GET_POINTER(x) && (((size_t) (x) & 3) == 3))	//pointer to heap
#define GET_INTEGER(x)			((size_t) (x) >> 2)
#define GET_POINTER(x)			(((size_t) (x) >> 2) << 2) // ((size_t) (x) & 0xFFFFFFFC)
#define MAKE_INTEGER(x)			((void *) (((size_t)(x) << 2) | 1))
#define	MAKE_POINTER_S(x)		((void *) (((size_t)(x)) | 2))
#define	MAKE_POINTER_H(x)		((void *) (((size_t)(x)) | 3))

#endif   /* ----- #ifndef MACRO_INC  ----- */
