#ifndef __TIMER_CAP_H__
#define __TIMER_CAP_H__

#include "typedef.h"
#include "asm/includes.h"


/**
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param cap_io : JL_PORTA_01, JL_PORTB_02,,,等等，支持任意普通IO
 * @param edge : 捕捉的触发边沿，0：上升沿触发  1：下降沿触发  2：上下沿切换触发,第一个边沿是上升沿  3：上下沿切换触发,第一个边沿是下降沿
 * @param clk_div : 时钟源的分频选择，分频越小，输入捕获的计数值数得越快，变化量就越大。时钟源统一选择std_24M
 * @param cbfun : 捕获起中断的中断回调函数，传出边沿和计数值
 */
void timer_cap_init(JL_TIMER_TypeDef *JL_TIMERx, u32 cap_io, u8 edge, CLK_DIV_4bit clk_div, void (*cbfun)(u8, u32));
/**
 * @brief 参考示例
 */
void timer_cap_test(void);


#endif
