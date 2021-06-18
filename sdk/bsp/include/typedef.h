/*************************************************************
File: typedef.h
Author:Juntham
Discriptor:
    数据类型重定义
Version:
Date：
*************************************************************/
#ifndef _typedef_h_
#define _typedef_h_

#include "asm/cpu.h"


///<locate code to x segment ever exist
#define SEC_USED(x)     __attribute__((section(#x),used))
#define SEC(x)          __attribute__((section(#x)))
#define sec(x)          __attribute__((section(#x),used))
#define ALIGNED(x)	    __attribute__((aligned(x)))
#define _GNU_PACKED_	__attribute__((packed))
#define _NOINLINE_	    __attribute__((noinline))
#define _INLINE_	    __attribute__((always_inline))
#define _WEAK_	        __attribute__((weak))
#define _WEAKREF_	    __attribute__((weakref))
#define _NORETURN_      __attribute__((noreturn))
#define _NAKED_         __attribute__((naked))



#undef FALSE
#define FALSE    	0

#undef TRUE
#define TRUE    	1

#define false    	0
#define true    	1

#ifndef NULL
#define NULL    	(void *)0
#endif


#define     BIT(n)              (1U << (n))
#define     BitSET(REG,POS)     ((REG) |= (1L << (POS)))
#define     BitCLR(REG,POS)     ((REG) &= (~(1L<< (POS))))
#define     BitXOR(REG,POS)     ((REG) ^= (~(1L << (POS))))
#define     BitCHK_1(REG,POS)   (((REG) & (1L << (POS))) == (1L << (POS)))
#define     BitCHK_0(REG,POS)   (((REG) & (1L << (POS))) == 0x00)
#define     testBit(REG,POS)    ((REG) & (1L << (POS)))

#define     clrBit(x,y)         (x) &= ~(1L << (y))
#define     setBit(x,y)         (x) |= (1L << (y))


#define readb(addr)   *((volatile unsigned char*)(addr))
#define readw(addr)   *((volatile unsigned short *)(addr))
#define readl(addr)   *((volatile unsigned long*)(addr))

#define writeb(addr, val)  *((volatile unsigned char*)(addr)) = (u8)(val)
#define writew(addr, val)  *((volatile unsigned short *)(addr)) = (u16)(val)
#define writel(addr, val)  *((volatile unsigned long*)(addr)) = (u32)(val)

#define ALIGN_4BYTE(size)   ((size+3)&0xfffffffc)


#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


#define ARRAY_SIZE(array)  (sizeof(array)/sizeof(array[0]))


#define likely(x) 	__builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define SFR(sfr, start, len, dat) \
	(sfr = (sfr & ~((~(0xffffffff << (len))) << (start))) | \
	 (((dat) & (~(0xffffffff << (len)))) << (start)))


#include "string.h"
#include "strings.h"
#include "errno-base.h"

#ifdef offsetof
#undef offsetof
#endif

#ifdef container_of
#undef container_of
#endif

#define offsetof(type, memb) \
	((unsigned long)(&((type *)0)->memb))

#define container_of(ptr, type, memb) \
	((type *)((char *)(ptr) - offsetof(type, memb)))

#endif



