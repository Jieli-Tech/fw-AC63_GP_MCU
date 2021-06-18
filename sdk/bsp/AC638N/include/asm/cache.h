//*********************************************************************************//
// Module name : cache.h                                                           //
//*********************************************************************************//

#ifndef __CACHE__
#define __CACHE__


void IcuEnable(void);

#define WAIT_ICACHE_IDLE()    \
    do { \
        asm volatile("csync"); \
        while (!(q32DSP_icu(core_num())->CON & BIT(31))); \
    } while(0)


void DcuEnable(void);

#define WAIT_DCACHE_IDLE()   \
    do { \
        asm volatile("csync");\
        while(!(JL_DCU->CON & BIT(31))); \
    } while(0);


#endif

