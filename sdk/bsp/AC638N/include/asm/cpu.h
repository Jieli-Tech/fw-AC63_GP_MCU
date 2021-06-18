
#ifndef ASM_CPU_H
#define ASM_CPU_H


#include "asm/AC638N.h"
#include "asm/csfr.h"
#include "asm/cache.h"

#ifndef __ASSEMBLY__

typedef unsigned char   		u8, bool, BOOL;
typedef char            		s8;
typedef unsigned short  		u16;
typedef signed short    		s16;
typedef unsigned int    		u32;
typedef signed int      		s32;
typedef unsigned long long 		u64;
typedef u32						FOURCC;
typedef long long               s64;
typedef unsigned long long      u64;


#endif



#ifndef BIG_ENDIAN
#define BIG_ENDIAN 			0x3021
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 		0x4576
#endif
#define CPU_ENDIAN 			LITTLE_ENDIAN



#ifndef __ASSEMBLY__

static inline int cpu_in_irq()
{
    int flag;
    __asm__ volatile("%0 = icfg" : "=r"(flag));
    return flag & 0xff;
}

static inline int cpu_irq_disabled()
{
    int flag;
    __asm__ volatile("%0 = icfg" : "=r"(flag));
    return (flag & 0x300) != 0x300;
}

static inline void delay(u32 cnt)
{
    while (cnt--) {
        asm("nop");
    }
}


static inline u32 rand32()
{
    return JL_RAND->R64L;
}


static inline void cpu_reset(void)
{
    void p33_soft_reset(void);
    p33_soft_reset();
}


#include "asm/irq.h"
#include "printf.h"



extern void local_irq_disable();
extern void local_irq_enable();




#define CPU_CRITICAL_ENTER()  local_irq_disable()

#define CPU_CRITICAL_EXIT()  local_irq_enable()


#define ASSERT(a,...)   \
		do { \
            if(!(a)){ \
                printf("file:%s, line:%d", __FILE__, __LINE__); \
                printf("ASSERT-FAILD: "#a" "__VA_ARGS__); \
                cpu_reset(); \
            } \
		}while(0);

#endif //__ASSEMBLY__


#endif

