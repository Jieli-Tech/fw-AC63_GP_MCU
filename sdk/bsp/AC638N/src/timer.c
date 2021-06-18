#include "asm/includes.h"
#include "asm/power_interface.h"

struct timer_hdl {
    u32 ticks;
    u8 run;
    void *power_ctrl;
};

static struct timer_hdl hdl;

#define __this  (&hdl)

static u32 cnt = 0;

#define MAX_TIME_CNT            0x7fff
#define MIN_TIME_CNT            0x100


static const u32 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};




___interrupt
static void timer1_isr()
{
    const struct timer_target *p;

    JL_TIMER1->CON |= BIT(14);

    list_for_each_timer_target(p) {
        if (p->timer_handle) {
            p->timer_handle();
        }
    }

    ++cnt;
#if (USE_KEY_DRIVER == 1) //10ms 按键扫描
    if ((cnt % 5) == 0) {
        key_driver_scan(&iokey_para);
    }
#endif
    if ((cnt % 50) == 0) {
        //100ms
    }
    if ((cnt % 100) == 0) {
        //200ms
    }
    if ((cnt % 250) == 0) {
        if (cnt == 500) {
            cnt = 0;
        }
        //500ms
#if 0
        powerdown_sleep();
#else
        printf("h");
#endif
    }

    /*lrc_scan();*/
}

int timer1_init(void)
{
    u8 index;
    u32 prd_cnt;

    request_irq(IRQ_TIME1_IDX, 1, timer1_isr, 0);

    u32 timer_clk = 24000000;
    for (index = 0; index < ARRAY_SIZE(timer_div); index++) {
        prd_cnt = 2 * (timer_clk / timer_div[index] / 1000);
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }

    JL_TIMER1->CNT = 0;
    JL_TIMER1->PRD = prd_cnt; //2ms
    JL_TIMER1->CON = BIT(14);
    JL_TIMER1->CON = (index << 4) | (6 << 10) | BIT(0);

    printf("timer1_prd: %d, %d\n", prd_cnt, index);

    __this->run = 1;

    return 0;
}

#ifndef time_after
#define time_after(a,b)  (((long)(b) - (long)(a)) < 0)
#endif

#ifndef time_before
#define time_before(a,b) time_after(b,a)
#endif

#define     SLEEP_TIMER JL_TIMER2

#define     CLR_PND     BIT(14)
#define     DIV8        0b0101

#define     TIMER_SRC_LSB       0
#define     TIMER_SRC_OSC       1
#define     TIMER_SRC_HTC       2
#define     TIMER_SRC_RC_16M    4
#define     TIMER_SRC_STD24     6

static int sleep_timer_ref = 0;

AT_VOLATILE_RAM_CODE
static void sleep_timer_init()
{
    SLEEP_TIMER->PRD = -1;
    SLEEP_TIMER->CNT = 0;
    SLEEP_TIMER->CON = CLR_PND | (DIV8 << 4) | (TIMER_SRC_STD24 << 10) | BIT(0);
}

AT_VOLATILE_RAM_CODE
void udelay(u32 us)
{
    local_irq_disable();
    if (sleep_timer_ref == 0) {
        sleep_timer_init();
    }
    sleep_timer_ref++;
    local_irq_enable();

    u32 ot = us * 3 + SLEEP_TIMER->CNT;
    while (1) {
        if (time_after(SLEEP_TIMER->CNT, ot)) {
            break;
        }
    }

    local_irq_disable();
    if (--sleep_timer_ref == 0) {
        SLEEP_TIMER->CON = BIT(14);
    }
    local_irq_enable();
}

AT_VOLATILE_RAM_CODE
void mdelay(u32 ms)
{
    for (int i = 0; i < ms; i++) {
        udelay(1000);
    }
}


/****************************************************************sleep_api********************************************************************************/

static void timer1_resume(void)
{
    if (__this->run == 0) {
        return;
    }

    u32 prd_cnt;
    u8 index;
    u32 timer_clk = 24000000;
    for (index = 0; index < ARRAY_SIZE(timer_div); index++) {
        prd_cnt = 2 * (timer_clk / timer_div[index] / 1000);
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }

    JL_TIMER1->CNT = __this->ticks;

    JL_TIMER1->PRD = prd_cnt; //2ms

    JL_TIMER1->CON = (index << 4) | (6 << 10) | BIT(0);
}

static void __power_suspend_post(void *priv, u32 usec)
{
    if (__this->run) {
        __this->ticks = JL_TIMER1->CNT;
        JL_TIMER1->CON = 0;
    }
}

static void __power_resume(void *priv, u32 usec)
{
    /*printf("cnt1 %d\n", cnt);*/
    cnt += usec / (1000L) / 2;
    /*printf("cnt2 %d\n", cnt);*/

    timer1_resume();
}

const struct low_power_operation timer_power_ops  = {
    .get_timeout 	= NULL,
    .suspend_probe 	= NULL,
    .suspend_post 	= __power_suspend_post,
    .resume 		= __power_resume,
};

void timer1_sleep_init(void)
{
    __this->power_ctrl = low_power_sys_get(NULL, &timer_power_ops);
}

