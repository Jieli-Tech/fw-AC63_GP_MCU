#ifndef __DEBUG_H__
#define __DEBUG_H__



void debug_init();

void debug_exception_analyze();

u32 debug_get_dev_id(char *name);

void debug_mpu_set(int idx, u32 begin, u32 end, u32 inv, const char *format, ...);


#endif


