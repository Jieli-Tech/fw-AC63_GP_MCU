#ifndef __CPU_CRC16_H__
#define __CPU_CRC16_H__


#include "typedef.h"



u16 CRC16(const void *ptr, u32 len);

/* i_val: CRC校验初值 */
u16 CRC16_with_initval(const void *ptr, u32 len, u16 i_val);



#endif


