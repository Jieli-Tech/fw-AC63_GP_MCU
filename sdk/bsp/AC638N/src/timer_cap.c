#include "asm/timer_cap.h"


static const u32 TIMERx_table[6] = {
    (u32)JL_TIMER0,
    (u32)JL_TIMER1,
    (u32)JL_TIMER2,
    (u32)JL_TIMER3,
    (u32)JL_TIMER4,
    (u32)JL_TIMER5,
};

static const u8 TIMERx_IRQ_IDX[6] = {
    IRQ_TIME0_IDX,
    IRQ_TIME1_IDX,
    IRQ_TIME2_IDX,
    IRQ_TIME3_IDX,
    IRQ_TIME4_IDX,
    IRQ_TIME5_IDX,
};

static u8 tmrcap_edge[6] = {0};
static void (*tmrcap_isr_cbfun[6])(u8 edge, u32 cnt) = {NULL};

/*
 * @brief   timer中断服务函数
*/
___interrupt
void timer_cap_isr(void)
{
    JL_TIMER_TypeDef *JL_TIMERx = NULL;
    u32 cur_cnt = 0;
    for (u8 tmr_num = 0; tmr_num < 6; tmr_num ++) {
        JL_TIMERx = (JL_TIMER_TypeDef *)(TIMERx_table[tmr_num]);
        if ((JL_TIMERx->CON & BIT(1)) && (JL_TIMERx->CON & BIT(15))) {
            JL_TIMERx->CON |= BIT(14);
            cur_cnt = JL_TIMERx->PRD;
            JL_TIMERx->CNT = 0;
            if (tmrcap_isr_cbfun[tmr_num]) {
                u8 cur_edge = !!(JL_TIMERx->CON & BIT(0));
                tmrcap_isr_cbfun[tmr_num](cur_edge, cur_cnt);
            }
            if (tmrcap_edge[tmr_num] > 1) {
                JL_TIMERx->CON ^= BIT(0);
            }
        }
    }
}

/**
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param cap_io : JL_PORTA_01, JL_PORTB_02,,,等等，支持任意普通IO
 * @param edge : 捕捉的触发边沿，0：上升沿触发  1：下降沿触发  2：上下沿切换触发,第一个边沿是上升沿  3：上下沿切换触发,第一个边沿是下降沿
 * @param clk_div : 时钟源的分频选择，分频越小，输入捕获的计数值数得越快，变化量就越大。时钟源统一选择std_24M
 * @param cbfun : 捕获起中断的中断回调函数，传出边沿和计数值
 */
void timer_cap_init(JL_TIMER_TypeDef *JL_TIMERx, u32 cap_io, u8 edge, CLK_DIV_4bit clk_div, void (*cbfun)(u8, u32))
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
    tmrcap_edge[tmr_num] = edge;
    tmrcap_isr_cbfun[tmr_num] = cbfun;
    request_irq(TIMERx_IRQ_IDX[tmr_num], 5, timer_cap_isr, 0);
    gpio_set_fun_input_port(cap_io, PFI_TMR0_CAP + 8 * tmr_num);
    //浮空输入
    gpio_set_die(cap_io, 1);
    gpio_set_direction(cap_io, 1);
    gpio_set_pull_up(cap_io, 0);
    gpio_set_pull_down(cap_io, 0);
    //初始化timer
    JL_TIMERx->CON = 0;
    JL_TIMERx->CON |= (6 << 10);						//时钟源选择STD_24M
    JL_TIMERx->CON |= ((clk_div & 0xf) << 4);			//设置时钟源分频
    JL_TIMERx->CNT = 0;								//清计数值
    JL_TIMERx->CON |= BIT(1);                          //输入捕获模式
    JL_TIMERx->CON += !!edge;
    if (edge == 3) {
        JL_TIMERx->CON &= ~BIT(0);
    }
}

/************************************ 以下SDK的参考示例 ****************************************/

static void user_tmr3_cap_cbfun(u8 edge, u32 cnt)
{
    printf("t3 %d %d\n", edge, cnt);
}
static void user_tmr4_cap_cbfun(u8 edge, u32 cnt)
{
    printf("t4 %d %d\n", edge, cnt);
}
void timer_cap_test(void)
{
    printf("*********** timer cap test *************\n");

    timer_cap_init(JL_TIMER3, IO_PORTC_02, 0, CLK_DIV_4, user_tmr3_cap_cbfun);

    timer_cap_init(JL_TIMER4, IO_PORTC_03, 2, CLK_DIV_8, user_tmr4_cap_cbfun);

    extern void wdt_clr();
    while (1) {
        wdt_clr();
    }
}

