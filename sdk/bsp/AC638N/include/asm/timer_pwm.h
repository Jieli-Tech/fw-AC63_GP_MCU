#ifndef __TIMER_PWM_H__
#define __TIMER_PWM_H__

#include "typedef.h"
#include "asm/includes.h"


/**
 * @brief timer_pwm初始化函数
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param pwm_io : JL_PORTA_01, JL_PORTB_02,,,等等，支持任意普通IO
 * @param fre : 频率，单位Hz
 * @param duty : 初始占空比，0~10000对应0~100%
 */
void timer_pwm_init(JL_TIMER_TypeDef *JL_TIMERx, u32 pwm_io, u32 fre, u32 duty);

/**
 * @brief timer_pwm改变占空比函数
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param duty : 占空比，0~10000对应0~100%
 */
void set_timer_pwm_duty(JL_TIMER_TypeDef *JL_TIMERx, u32 duty);

/**
 * @brief 参考示例
 */
void timer_pwm_test(void);


#endif


