#ifndef __IRFLT_H__
#define __IRFLT_H__

#include "typedef.h"
#include "asm/includes.h"

typedef struct _IR_CODE {
    u16 wData;          //<键值
    u16 wUserCode;      //<用户码
    u16 timer_pad;      //<当前捕捉值
    u8  bState;         //<接收状态
    u8  boverflow;      //<红外信号超时
} IR_CODE;

struct irflt_platform_data {
    u8 ir_io;           //红外捕捉引脚
    u8 timer_num;       //选用哪个timer来做捕捉
};

#define IRFLT_PLATFORM_DATA_BEGIN(data) \
		static const struct irflt_platform_data data = {

#define IRFLT_PLATFORM_DATA_END() \
};

/*
 * @brief   获取timer输入捕捉红外解码的数值
 * @return  解码后的值
*/
u8 get_irflt_value(void);
/*
 * @brief  IO复用时，IO挂起函数
 * @return 0：挂起成功，  1：挂起是吧
*/
u8 ir_io_suspend(void);

/*
 * @brief  IO复用时，IO恢复函数
 * return  0：恢复成功
*/
u8 ir_io_resume(void);

/*
 * @brief  红外初始化函数
 * @arg pdata 初始化的参数结构体地址 ： struct irflt_platform_data *
*/
void irflt_init(void *pdata);

/*
 * @brief  参考示例函数
*/
void irflt_test(void);


#endif

