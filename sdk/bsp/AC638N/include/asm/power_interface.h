#ifndef POWER_INTERFACE_H
#define POWER_INTERFACE_H

// #include "asm/hwi.h"
//
#include "typedef.h"
#define NEW_BASEBAND_COMPENSATION       0

#define AT_VOLATILE_RAM             sec(.volatile_ram)
#define AT_VOLATILE_RAM_CODE        sec(.volatile_ram_code)

enum {
    /*主系统*/
    MSYS_P11_RST = 0,
    MSYS_DVDD_POR_RST = 1,
    MSYS_DVDD_OK_RST = 2,
    MSYS_SOFT_RST = 5,
    MSYS_P2M_RST = 6,
    MSYS_POWER_RETURN = 7,
    /*P11*/
    P11_PVDD_POR_RST = 8,
    P11_IVS_RST = 9,
    P11_P33_RST = 10,
    P11_WDT_RST = 11,
    P11_SOFT_RST = 12,
    P11_MSYS_RST = 13,
    P11_POWER_RETURN = 15,
    /*P33*/
    P33_VDDIO_POR_RST = 16,
    P33_VDDIO_LVD_RST = 17,
    P33_VCM_RST = 18,
    P33_PPINR_RST = 19,
    P33_P11_RST = 20,
    P33_SOFT_RST = 21,
    P33_POWER_RETURN = 23,
    /*SUB*/
    P33_EXCEPTION_SOFT_RST = 24,
};

enum {
    OSC_TYPE_LRC = 0,
    OSC_TYPE_BT_OSC,
};

enum {
    PWR_NO_CHANGE = 0,
    PWR_LDO33,
    PWR_LDO15,
    PWR_DCDC15,
    PWR_DCDC15_FOR_CHARGE,
};

enum {
    LONG_4S_RESET = 0,
    LONG_8S_RESET,
};

enum {
    VDDIOM_VOL_20V = 0,
    VDDIOM_VOL_22V,
    VDDIOM_VOL_24V,
    VDDIOM_VOL_26V,
    VDDIOM_VOL_28V,
    VDDIOM_VOL_30V, //default
    VDDIOM_VOL_32V,
    VDDIOM_VOL_34V,
};

enum {
    VDDIOW_VOL_20V = 0,
    VDDIOW_VOL_22V,
    VDDIOW_VOL_24V,
    VDDIOW_VOL_26V,
    VDDIOW_VOL_28V,
    VDDIOW_VOL_30V,
    VDDIOW_VOL_32V,
    VDDIOW_VOL_34V,
};

struct low_power_param {
    u8 osc_type;
    u32 btosc_hz;
    u8  delay_us;
    u8  config;
    u8  btosc_disable;

    u8 vddiom_lev;
    u8 vddiow_lev;
    u8 pd_wdvdd_lev;
    u8 lpctmu_en;
    u8 vddio_keep;

    u32 osc_delay_us;

    u8 rtc_clk;
    u8 light_sleep_attribute;
};


#define RISING_EDGE         0
#define FALLING_EDGE        1

enum {
    PORT_FLT_NULL = 0,
    PORT_FLT_256us,
    PORT_FLT_512us,
    PORT_FLT_1ms,
    PORT_FLT_2ms,
    PORT_FLT_4ms,
    PORT_FLT_8ms,
    PORT_FLT_16ms,
};

struct port_wakeup {
    u8 pullup_down_enable;        //
    u8 edge;        //[0]: Rising / [1]: Falling
    u8 both_edge;
    u8 filter;
    u8 iomap;       //Port Group-Port Index
};


struct lvd_wakeup {
    u8 attribute;   //Relate operation bitmap OS_RESUME | BLUETOOTH_RESUME
};


//<Max hardware wakeup port
#define MAX_WAKEUP_PORT     8

struct wakeup_param {
    struct port_wakeup *port[MAX_WAKEUP_PORT];
    const struct charge_wakeup *charge;
    const struct alarm_wakeup *alram;
    const struct lvd_wakeup *lvd;
    const struct sub_wakeup *sub;
};

struct reset_param {
    u8 en;
    u8 mode;
    u8 level;
    u8 iomap;   //Port Group, Port Index
};

struct low_power_operation {

    const char *name;

    u32(*get_timeout)(void *priv);

    void (*suspend_probe)(void *priv);

    void (*suspend_post)(void *priv, u32 usec);

    void (*resume)(void *priv, u32 usec);

    void (*resume_post)(void *priv, u32 usec);
};
/*-----------------------------------------------------------------------------------
*								power_api
-------------------------------------------------------------------------------------*/
/**@brief    电源初始化
   @param
   @return
   @note
 */
void p11_init(void);

/**@brief    电源初始化
   @param    low_power_param
   @return
   @note
 */
void power_init(const struct low_power_param *param);

/**@brief  配置电源模式
   @param  PWR_LDO15、PWR_DCDC15、PWR_DCDC15_FOR_CHARGE
   @return
   @note
 */
void power_set_mode(u8 mode);
/**@brief  配置为充电状态模式
   @param  1:充电状态   0:非充电状态
   @return
   @note
 */
void power_set_charge_mode(u8 mode);

struct soft_flag0_t {
    u8 wdt_dis: 1;
    u8 poweroff: 1;
    u8 is_port_b: 1;
    u8 res: 5;
};
struct soft_flag1_t {
    u8 usbdp: 2;
    u8 usbdm: 2;
    u8 uart_key_port: 1;
    u8 ldoin: 3;
};
struct soft_flag2_t {
    u8 pa7: 4;
    u8 pb4: 4;
};
struct soft_flag3_t {
    u8 pc3: 4;
    u8 pc5: 4;
};

struct boot_soft_flag_t {
    union {
        struct soft_flag0_t boot_ctrl;
        u8 value;
    } flag0;
    union {
        struct soft_flag1_t misc;
        u8 value;
    } flag1;
    union {
        struct soft_flag2_t pa7_pb4;
        u8 value;
    } flag2;
    union {
        struct soft_flag3_t pc3_pc5;
        u8 value;
    } flag3;
};
enum soft_flag_io_stage {
    SOFTFLAG_HIGH_RESISTANCE,
    SOFTFLAG_PU,
    SOFTFLAG_PD,

    SOFTFLAG_OUT0,
    SOFTFLAG_OUT0_HD0,
    SOFTFLAG_OUT0_HD,
    SOFTFLAG_OUT0_HD0_HD,

    SOFTFLAG_OUT1,
    SOFTFLAG_OUT1_HD0,
    SOFTFLAG_OUT1_HD,
    SOFTFLAG_OUT1_HD0_HD,
};
/**@brief   softoff参数配置,例如配置启动时rom io状态是否保持
   @param
   @return
   @note
 */
void mask_softflag_config(const struct boot_soft_flag_t *softflag);
/**@brief   配置softoff、powerdown回调函数
   @param
   @return
   @note
 */
void power_set_callback(u8 mode, void (*powerdown_enter)(u8 step), void (*powerdown_exit)(u32), void (*soft_poweroff_enter)(void));

/**@brief   复位源log
   @param
   @return
   @note
 */
u32 power_reset_source_dump(void);

/**@brief   系统软关机调用接口
   @param
   @return
   @note
 */
void power_set_soft_poweroff();
/**@brief   系统powerdown lower_power_operation调用接口
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
void powerdown_sleep();
/**@brief   系统powerdown lower_power_operation调用接口
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
void low_power_sys_request(void *priv);
/**@brief   系统powerdown lower_power_operation调用接口
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
void *low_power_sys_get(void *priv, const struct low_power_operation *ops);
/**@brief   系统powerdown lower_power_operation调用接口
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
void low_power_sys_put(void *priv);
/**@brief   系统powerdown lower_power_operation调用接口
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
u8 low_power_sys_is_idle(void);
/**@brief   系统powerdown lower_power_operation调用接口，配置powerdown晶振类型
   @param
   @return
   @note    如何注册为系统注册low_power_operation参考src/tick_timer.c
 */
void low_power_reset_osc_type(u8 type);
/**@brief   系统powerdown调用接口，获取powerdown默认晶振类型
   @param
   @return
   @note
 */
u8 low_power_get_default_osc_type(void);
/**@brief   系统powerdown调用接口，获取powerdown当前晶振类型
   @param
   @return
   @note
 */
u8 low_power_get_osc_type(void);

/**@brief   系统powerdown调用接口，获取powerdown当前晶振类型
   @param
   @return
   @note
 */

/*-----------------------------------------------------------------------------------
*								wakeup_api
-------------------------------------------------------------------------------------*/
/**@brief   唤醒选择器使能
   @param  	选择器索引
   @return
   @note
 */
void power_wakeup_index_enable(u8 index);
/**@brief   唤醒选择器失能
   @param  	选择器索引
   @return
   @note
 */
void power_wakeup_index_disable(u8 index);
/**@brief   唤醒端口使能
   @param  	唤醒端口
   @return
   @note
 */
void power_wakeup_enable_with_port(u8 port);
/**@brief   唤醒端口失能
   @param  	唤醒端口
   @return
   @note
 */
void power_wakeup_disable_with_port(u8 port);
/**@brief   唤醒端口边沿唤醒配置
   @param  	port_io:唤醒端口 edge:边沿类型(RISING_EGE、FALLING_EDGE)
   @return
   @note
 */
void power_wakeup_set_edge(u8 port_io, u8 edge);
/**@brief   所有唤醒选择器初始化
   @param  	所有唤醒端口参数
   @return
   @note
 */
void power_wakeup_init(const struct wakeup_param *param);
/**@brief   配置边沿唤醒回调
   @param  	唤醒选择器、端口号
   @return
   @note
 */
void port_edge_wkup_set_callback(void (*wakeup_callback)(u8 index, u8 gpio));
/**@brief  	唤醒选择器初始化
   @param  	唤醒选择器、端口参数
   @return
   @note
 */
void power_wakeup_port_set(u8 idx, struct port_wakeup *port);
/**@brief  	获取当前唤醒类型
   @param
   @return  唤醒类型
   @note
 */
u8 get_wakeup_source(void);
/**@brief  	获取当前唤醒类型是否为ldo5v
   @param
   @return  1.是  2.否
   @note
 */
u8 is_ldo5v_wakeup(void);
/**@brief  	获取当前唤醒类型是否为alarm
   @param
   @return  1.是  2.否
   @note
 */
u8 is_alarm_wakeup(void);
/**@brief   软件复位
   @param
   @return
   @note
 */
void p33_soft_reset(void);
/**@brief   SDC电源配置
   @param
   @return
   @note
 */
void sdpg_config(int enable);
/**@brief   获取复位源
   @param
   @return  复位源
   @note
 */
u32 get_reset_source_value(void);
/**@brief
   @param
   @return
   @note
 */
void lrc_scan();

/**
 * @brief power_trim 校准芯片内部wvdd pvdd电源电压
 * 会记录数据到vm_id 里面
 *
 * @param vddio_vbat_tieup 1:vddio & vbat 短接 0，vddio vbat 不短接
 */
void power_trim(u32 vm_id, u32 vddio_vbat_tieup);
//配置Low power target 睡眠深度
// -- LOW_POWER_MODE_SLEEP : 系统掉电，RAM 进入省电模式，数字逻辑不掉电，模拟掉电
// -- LOW_POWER_MODE_LIGHT_SLEEP : 系统不掉电，BTOSC 保持，系统运行RC
// -- LOW_POWER_MODE_DEEP_SLEEP : 数字逻辑不掉电，模拟掉电
enum LOW_POWER_LEVEL {
    LOW_POWER_MODE_SLEEP = 0,
    LOW_POWER_MODE_LIGHT_SLEEP,
    LOW_POWER_MODE_DEEP_SLEEP,
};

#define LOWPOWER_LIGHT_SLEEP_ATTRIBUTE_KEEP_CLOCK 		BIT(0)

typedef u8(*idle_handler_t)(void);
typedef enum LOW_POWER_LEVEL(*level_handler_t)(void);

struct lp_target {
    char *name;
    level_handler_t level;
    idle_handler_t is_idle;
};

#define REGISTER_LP_TARGET(target) \
        const struct lp_target target sec(.lp_target)


extern const struct lp_target lp_target_begin[];
extern const struct lp_target lp_target_end[];

#define list_for_each_lp_target(p) \
    for (p = lp_target_begin; p < lp_target_end; p++)

struct deepsleep_target {
    char *name;
    u8(*enter)(void);
    u8(*exit)(void);
};

#define DEEPSLEEP_TARGET_REGISTER(target) \
        const struct deepsleep_target target sec(.deepsleep_target)


extern const struct deepsleep_target deepsleep_target_begin[];
extern const struct deepsleep_target deepsleep_target_end[];

#define list_for_each_deepsleep_target(p) \
    for (p = deepsleep_target_begin; p < deepsleep_target_end; p++)
/*-----------------------------------------------------------*/

#endif
