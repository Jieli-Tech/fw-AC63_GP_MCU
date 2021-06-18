#include "asm/clock.h"
#include "asm/timer.h"
#include "asm/power/p33.h"
#include "asm/charge.h"
#include "asm/power_interface.h"
#include "asm/efuse.h"
#include "asm/gpio.h"

typedef struct _CHARGE_VAR {
    struct charge_platform_data *data;
    u8 charge_online_flag : 1;
    u8 init : 1;
    u8 vbat_isr : 1;
    u8 vpwr_isr : 1;
    u8 charge_flag : 2;
} CHARGE_VAR;

#define __this 	(&charge_var)
static CHARGE_VAR charge_var;
extern void charge_set_callback(void (*wakup_callback)(void), void (*sub_callback)(void));

#define BIT_VPWR_IN			1
#define BIT_VPWR_OFF		2
#define BIT_VPWR_KEEP		3

/*----------------------------------------------------*/
/**@brief    获取VPWR在线状态
   @param    无
   @return   0:不在线 1:在线
   @note     VPWR电压大于插入电压（约0.6V）则返回值为1
*/
/*----------------------------------------------------*/
u8 charge_get_online_flag(void)
{
    return __this->charge_online_flag;
}

/*----------------------------------------------------*/
/**@brief    获取VPWR和插入电压（约0.6V）比的大小
   @param    无
   @return   0:VPWR<0.6 1:VPWR>0.6
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
u8 charge_get_vpwr_online_hw(void)
{
    return LDO5V_DET_GET();
}

/*----------------------------------------------------*/
/**@brief    获取VPWR和VBAT比的大小
   @param    无
   @return   0:VPWR<VBAT 1:VPWR>VBAT
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
u8 charge_get_lvcmp_det(void)
{
    return LVCMP_DET_GET();
}

/*----------------------------------------------------*/
/**@brief    soft power off时,判定是否需要维持下拉电阻
   @param    无
   @return   0:关闭下拉电阻 1:使能下拉电阻
   @note     该接口由软关机函数调用
*/
/*----------------------------------------------------*/
u8 charge_get_vpwr_pulldown_en(void)
{
    if (!__this->data) {
        return 0;
    }
    if (__this->data->vpwr_pulldown_keep == 0) {
        return 0;
    }
    return __this->data->vpwr_pulldown_en;
}

/*----------------------------------------------------*/
/**@brief    soft power off时,获取下拉电阻档位
   @param    无
   @return   0:50k 1:100k 2:150k 3:200k
   @note     该接口由软关机函数调用
*/
/*----------------------------------------------------*/
u8 charge_get_vpwr_pulldown_res(void)
{
    if (__this->data) {
        return __this->data->vpwr_pulldown_lvl;
    }
    return CHARGE_PULLDOWN_200K;
}

/*----------------------------------------------------*/
/**@brief    获取配置的恒流充电电流档位
   @param    无
   @return   参考charge.h充电电流档位
   @note     该接口用于获取配置的档位
*/
/*----------------------------------------------------*/
u8 charge_get_mA_config(void)
{
    return __this->data->charge_mA;
}

/*----------------------------------------------------*/
/**@brief    设置恒流充电电流档位
   @param    参考charge.h充电电流档位
   @return   无
   @note     该接口用于改变充电电流档位,使用场景有:
             1、电池电压小时(<3V),用小电流充电(20mA)
                电池电压大时(>3V),用配置的电流充电(charge_mA)
             2、温度升高时,降低充电电流,减小发热
*/
/*----------------------------------------------------*/
void charge_set_mA(u8 charge_mA)
{
    static u8 charge_mA_old = 0xff;
    if (charge_mA_old != charge_mA) {
        charge_mA_old = charge_mA;
        CHARGE_mA_SEL(charge_mA);
    }
}

/*----------------------------------------------------*/
/**@brief    开始充电
   @param    无
   @return   无
   @note     当VPWR电压大于电池电压时,可进行充电
*/
/*----------------------------------------------------*/
void charge_start(void)
{
    printf("%s\n", __func__);
    if (__this->data->pwr_sel == PWR_DCDC15) {
        power_set_charge_mode(1);
        power_set_mode(PWR_DCDC15_FOR_CHARGE);
    }
    CHGBG_EN(1);
    CHARGE_EN(1);
}

/*----------------------------------------------------*/
/**@brief    停止充电
   @param    无
   @return   无
   @note     当VPWR电压小于于电池电压时,可停止充电
*/
/*----------------------------------------------------*/
void charge_close(void)
{
    printf("%s\n", __func__);
    CHGBG_EN(0);
    CHARGE_EN(0);
    if (__this->data->pwr_sel == PWR_DCDC15) {
        power_set_charge_mode(0);
        power_set_mode(__this->data->pwr_sel);
    }
}

/*----------------------------------------------------*/
/**@brief    充满检测
   @param    无
   @return   无
   @note     当充电电流小于配置的充满电流时,充电判满
*/
/*----------------------------------------------------*/
static void charge_full_detect(void)
{
    static u16 charge_full_cnt = 0;

    if (__this->vbat_isr == 0) {
        return;
    }

    if (IS_CHARGE_EN() == 0) {
        __this->vbat_isr = 0;
        return;
    }

    if (CHARGE_FULL_FLAG_GET() && LVCMP_DET_GET()) {
        /* putchar('F'); */
        if (charge_full_cnt < __this->data->charge_full_filter) {
            charge_full_cnt++;
        } else {
            charge_full_cnt = 0;
            printf("Charge Full!\r\n");
            charge_close();
            __this->vbat_isr = 0;
        }
    } else {
        /* putchar('K'); */
        charge_full_cnt = 0;
        __this->vbat_isr = 0;
        power_wakeup_enable_with_port(IO_CHGFL_DET);
    }
}

/*----------------------------------------------------*/
/**@brief    VPWR插拔检测
   @param    无
   @return   无
   @note     当VPWR电压不同时,改变检测状态,开启充电或关闭充电
*/
/*----------------------------------------------------*/
static void charge_vpwr_detect(void)
{
    static u16 vpwr_in_normal_cnt = 0;
    static u16 vpwr_in_keep_cnt = 0;
    static u16 vpwr_off_cnt = 0;

    if (__this->vpwr_isr == 0) {
        return;
    }

    if (LVCMP_DET_GET()) {
        //VPWR > vbat
        /* putchar('X'); */
        if (vpwr_in_normal_cnt < __this->data->vpwr_on_filter) {
            vpwr_in_normal_cnt++;
        } else {
            printf("VPWR_IN\r\n");
            __this->charge_online_flag = 1;
            if (__this->charge_flag != BIT_VPWR_IN) {
                __this->charge_flag = BIT_VPWR_IN;
                power_wakeup_set_edge(IO_VBTCH_DET, FALLING_EDGE);
                charge_start();
            }
            goto __clr_exit;
        }
    } else if (LDO5V_DET_GET() == 0) {
        //VPWR<拔出电压（0.6）
        /* putchar('Q'); */
        if (vpwr_off_cnt < (__this->data->vpwr_off_filter)) {
            vpwr_off_cnt++;
        } else {
            printf("VPWR_OFF\r\n");
            __this->charge_online_flag = 0;
            if (__this->charge_flag != BIT_VPWR_OFF) {
                __this->charge_flag = BIT_VPWR_OFF;
                power_wakeup_set_edge(IO_VBTCH_DET, RISING_EDGE);
                charge_close();
            }
            goto __clr_exit;
        }
    } else {
        //拔出电压（0.6左右）< VPWR < vbat
        /* putchar('E'); */
        if (vpwr_in_keep_cnt < __this->data->vpwr_keep_filter) {
            vpwr_in_keep_cnt++;
        } else {
            printf("VPWR_KEEP\r\n");
            __this->charge_online_flag = 1;
            if (__this->charge_flag == BIT_VPWR_KEEP) {
                __this->charge_flag = BIT_VPWR_KEEP;
                power_wakeup_set_edge(IO_VBTCH_DET, RISING_EDGE);//拔出后重新检测插入
                charge_close();
            }
            goto __clr_exit;
        }
    }
    return;
__clr_exit:
    vpwr_off_cnt = 0;
    vpwr_in_normal_cnt = 0;
    vpwr_in_keep_cnt = 0;
    __this->vpwr_isr = 0;
    return;
}

/*----------------------------------------------------*/
/**@brief    检测定时器
   @param    无
   @return   无
   @note     调用VPWR插拔检测和充电时的充满检测
*/
/*----------------------------------------------------*/
static void charge_timer_handle(void)
{
    if (!__this->init) {
        return;
    }
    charge_vpwr_detect();
    charge_full_detect();
}
REGISTER_TIMER_TARGET(charge_timer) = {
    .timer_handle = charge_timer_handle,
};

/*----------------------------------------------------*/
/**@brief    进入低功耗时检测是否允许进入
   @param    无
   @return   0:禁止进低功耗 1:允许进低功耗
   @note     无
*/
/*----------------------------------------------------*/
static u8 charge_check_is_idle(void)
{
    return ((__this->vpwr_isr == 0) && (__this->vbat_isr == 0));
}
REGISTER_LP_TARGET(charge_is_idle_target) = {
    .name = "charge",
    .is_idle = charge_check_is_idle,
};

/*----------------------------------------------------*/
/**@brief    充电口边沿唤醒回调接口
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
static void vpwr_wakeup_isr(void)
{
    __this->vpwr_isr = 1;
}

/*----------------------------------------------------*/
/**@brief    充满标记唤醒系统回调接口
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
static void charge_wakeup_isr(void)
{
    power_wakeup_disable_with_port(IO_CHGFL_DET);
    __this->vbat_isr = 1;
}

/*----------------------------------------------------*/
/**@brief    充电配置
   @param    无
   @return   无
   @note     根据efuse的trim值,设置充满电压寄存器;
             根据配置设置充满电流阈值;
             根据配置设置恒流充电电流
*/
/*----------------------------------------------------*/
static void charge_config(void)
{
    u8 charge_trim_val = CHARGE_FULL_V_4222;
    u8 offset = 0;
    u8 charge_full_v_val = 0;

    if (get_vbat_trim() == 0xf) {
        printf("vbat not trim, use default config!!!!!!");
    } else {
        charge_trim_val = get_vbat_trim();//4.2V对应的trim出来的实际档位
    }

    printf("charge_trim_val = %d\n", charge_trim_val);

    if (__this->data->charge_full_V >= CHARGE_FULL_V_4222) {
        offset = __this->data->charge_full_V - CHARGE_FULL_V_4222;
        charge_full_v_val = charge_trim_val + offset;
        if (charge_full_v_val > 0xf) {
            charge_full_v_val = 0xf;
        }
    } else {
        offset = CHARGE_FULL_V_4222 - __this->data->charge_full_V;
        if (charge_trim_val >= offset) {
            charge_full_v_val = charge_trim_val - offset;
        } else {
            charge_full_v_val = 0;
        }
    }

    printf("charge_full_v_val = %d\n", charge_full_v_val);

    CHARGE_FULL_V_SEL(charge_full_v_val);
    CHARGE_FULL_mA_SEL(__this->data->charge_full_mA);
    CHARGE_mA_SEL(__this->data->charge_mA);
}


/*----------------------------------------------------*/
/**@brief    内置充电初始化
   @param    配置结构体
   @return   无
   @note     设置充电相关寄存器
*/
/*----------------------------------------------------*/
void charge_init(void *arg)
{
    printf("%s\r\n", __func__);

    __this->data = (struct charge_platform_data *)arg;

    ASSERT(__this->data);

    __this->charge_online_flag = 0;
    __this->charge_flag = BIT_VPWR_OFF;

    /*先关闭充电使能，后面检测到充电插入再开启*/
    CHGBG_EN(0);
    CHARGE_EN(0);

    /*VPWR的100K下拉电阻使能*/
    L5V_RES_DET_S_SEL(__this->data->vpwr_pulldown_lvl);
    L5V_LOAD_EN(__this->data->vpwr_pulldown_en);

    charge_config();

    charge_set_callback(charge_wakeup_isr, vpwr_wakeup_isr);

    __this->init = 1;

    __this->vpwr_isr = 1;
}

