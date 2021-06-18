#include "asm/timer_pwm.h"


static const u32 TIMERx_table[6] = {
    (u32)JL_TIMER0,
    (u32)JL_TIMER1,
    (u32)JL_TIMER2,
    (u32)JL_TIMER3,
    (u32)JL_TIMER4,
    (u32)JL_TIMER5,
};

/**
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param pwm_io : JL_PORTA_01, JL_PORTB_02,,,等等，支持任意普通IO
 * @param fre : 频率，单位Hz
 * @param duty : 初始占空比，0~10000对应0~100%
 */
void timer_pwm_init(JL_TIMER_TypeDef *JL_TIMERx, u32 pwm_io, u32 fre, u32 duty)
{
    u8 tmr_num;
    for (tmr_num = 0; tmr_num < 6; tmr_num ++) {
        if ((u32)JL_TIMERx == TIMERx_table[tmr_num]) {
            break;
        }
        if (tmr_num == 5) {
            return;
        }
    }
    u32 timer_clk = 24000000;
    if (tmr_num == 3) {
        bit_clr_ie(IRQ_TIME3_IDX);
    }
    gpio_set_fun_output_port(pwm_io, FO_TMR0_PWM + tmr_num, 0, 1);
    gpio_set_die(pwm_io, 1);
    gpio_set_pull_up(pwm_io, 0);
    gpio_set_pull_down(pwm_io, 0);
    gpio_set_direction(pwm_io, 0);
    //初始化timer
    JL_TIMERx->CON = 0;
    JL_TIMERx->CON |= (6 << 2);						//时钟源选择STD_24M
    JL_TIMERx->CON |= (0b0001 << 4);					//时钟源再4分频
    JL_TIMERx->CNT = 0;								//清计数值
    JL_TIMERx->PRD = timer_clk / (4 * fre);			//设置周期
    //设置初始占空比
    JL_TIMERx->PWM = (JL_TIMERx->PRD * duty) / 10000;	//0~10000对应0~100%

    JL_TIMERx->CON |= BIT(8) | (0b01 << 0); 			//计数模式
}

/**
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param duty : 占空比，0~10000对应0~100%
 */
void set_timer_pwm_duty(JL_TIMER_TypeDef *JL_TIMERx, u32 duty)
{
    JL_TIMERx->PWM = (JL_TIMERx->PRD * duty) / 10000;	//0~10000对应0~100%
}


/********************************* 以下SDK的参考示例 ****************************/

void timer_pwm_test(void)
{
    printf("*********** timer pwm test *************\n");

    timer_pwm_init(JL_TIMER4, IO_PORTC_02, 1000, 5000); //1KHz 50%
    timer_pwm_init(JL_TIMER5, IO_PORTC_03, 10000, 7500);//10KHz 75%

    extern void wdt_clr();
    while (1) {
        wdt_clr();
    }
}
