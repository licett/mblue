/***************************************************************************
 **
 **    Common definition for IAR EW ARM
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2006
 **
 **    $Revision: 48478 $
 **
 ***************************************************************************/
//#include <intrinsics.h>

#ifndef __MBLUE_COMM_DEF_H
#define __MBLUE_COMM_DEF_H

#define MHZ           *1000000l
#define KHZ           *1000l
#define HZ            *1l

typedef enum {
	FALSE, TRUE
} BOOL;

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef double                Flo64;    // Double precision floating point
typedef double              * pFlo64;
typedef float                 Flo32;    // Single precision floating point
typedef float               * pFlo32;
typedef signed   long long    Int64S;   // Signed   64 bit quantity
typedef signed   long long  * pInt64S;
typedef unsigned long long    Int64U;   // Unsigned 64 bit quantity
typedef unsigned long long  * pInt64U;
typedef signed   int          Int32S;   // Signed   32 bit quantity
typedef signed   int        * pInt32S;
typedef unsigned int          Int32U;   // Unsigned 32 bit quantity
typedef unsigned int        * pInt32U;
typedef signed   short        Int16S;   // Signed   16 bit quantity
typedef signed   short      * pInt16S;
typedef unsigned short        Int16U;   // Unsigned 16 bit quantity
typedef unsigned short      * pInt16U;
typedef signed   char         Int8S;    // Signed    8 bit quantity
typedef signed   char       * pInt8S;
typedef unsigned char         Int8U;    // Unsigned  8 bit quantity
typedef unsigned char       * pInt8U;
typedef unsigned int          Boolean;  // Boolean
typedef unsigned int        * pBoolean;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _2BL(a)   (Int8U)(a),(Int8U)(a>>8)
#define _2BB(a)   (Int8U)(a>>8),(Int8U)(a),
#define _3BL(a)   (Int8U)(a),(Int8U)(a>>8),(Int8U)(a>>16)
#define _3BB(a)   (Int8U)(a>>16),(Int8U)(a>>8),(Int8U)(a)
#define _4BL(a)   (Int8U)(a),(Int8U)(a>>8),(Int8U)(a>>16),(Int8U)(a>>24)
#define _4BB(a)   (Int8U)(a>>24),(Int8U)(a>>16),(Int8U)(a>>8),(Int8U)(a)

typedef void * (*CommUserFpnt_t)(void *);
typedef void   (*VoidFpnt_t)(void);


#define LongToBin(n) (((n >> 21) & 0x80) | \
                      ((n >> 18) & 0x40) | \
                      ((n >> 15) & 0x20) | \
                      ((n >> 12) & 0x10) | \
                      ((n >>  9) & 0x08) | \
                      ((n >>  6) & 0x04) | \
                      ((n >>  3) & 0x02) | \
                      ((n      ) & 0x01))

#define __BIN(n) LongToBin(0x##n##l)

#define BIN8(n)                       __BIN(n)
#define BIN(n)                        __BIN(n)
#define BIN16(b1,b2)        ((        __BIN(b1)  <<  8UL) + \
                                      __BIN(b2))
#define BIN32(b1,b2,b3,b4) ((((Int32U)__BIN(b1)) << 24UL) + \
                            (((Int32U)__BIN(b2)) << 16UL) + \
                            (((Int32U)__BIN(b3)) <<  8UL) + \
                              (Int32U)__BIN(b4))

enum led_action_type {
	LED_ACTION_SLEEP,
	LED_ACTION_CONNECTING,
	LED_ACTION_CONNECTED,
	LED_ACTION_MAX
};

#endif // __MBLUE_COMM_DEF_H
