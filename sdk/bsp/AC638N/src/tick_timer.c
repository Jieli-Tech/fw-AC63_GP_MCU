#include "asm/cpu.h"
#include "asm/power_interface.h"

void tick_timer_init(void)
{


}

/***********************************************sleep_api*********************************************************/
static void *sys_low_power;

static volatile u8 sys_low_power_request = 0;

void powerdown_sleep()
{
    if (sys_low_power_request == 0) {
        /* log_info("xExpectedIdleTime : %d", xExpectedIdleTime); */
        low_power_sys_request(NULL);
    }
}

u32 __attribute__((weak)) return_power_timeout(void)
{
    return -2; //默认等待模式
}

static u32 __power_get_timeout(void *priv)
{
    if (low_power_sys_is_idle() == 0) {
        /* log_error("low_power_sys_is_idle"); */
        return 0;
    }

    return return_power_timeout();
}

static void __power_suspend_post(void *priv, u32 usec)
{
    sys_low_power_request = 1;
}

static void __power_resume(void *priv, u32 usec)
{
    sys_low_power_request = 0;
}

const struct low_power_operation sys_power_ops  = {
    .get_timeout 	= __power_get_timeout,

    .suspend_probe 	= NULL,
    .suspend_post 	= __power_suspend_post,
    .resume 		= __power_resume,
};

static void vPortPowerInit(void)
{
    sys_low_power = low_power_sys_get(NULL, &sys_power_ops);
}

void vPortSysSleepInit(void)
{
    vPortPowerInit();
}



