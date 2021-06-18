#include "asm/crc16.h"
#include "asm/cpu.h"

#define crc16_mutex_create()
#define crc16_mutex_pend()
#define crc16_mutex_post()


int crc16_init()
{
    crc16_mutex_create();
    return 0;
}

u16 CRC16(const void *ptr, u32 len)
{
    u16 ret;
    u8 *p = (u8 *)ptr;

    crc16_mutex_pend();

    JL_CRC->REG = 0;
    while (len--) {
        JL_CRC->FIFO = *p++;
    }
    asm volatile("csync");
    ret = JL_CRC->REG;


    crc16_mutex_post();

    return ret;
}

/* i_val: CRC校验初值 */
u16 CRC16_with_initval(const void *ptr, u32 len, u16 i_val)
{
    u16 ret;
    u8 *p = (u8 *)ptr;

    crc16_mutex_pend();

    JL_CRC->REG = i_val;
    while (len--) {
        JL_CRC->FIFO = *p++;
    }
    asm volatile("csync");
    ret = JL_CRC->REG;


    crc16_mutex_post();

    return ret;
}


