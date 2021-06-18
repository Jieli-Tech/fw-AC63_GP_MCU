#ifndef _CPU_CLOCK_
#define _CPU_CLOCK_

#include "typedef.h"

#include "clock_hw.h"

///原生时钟源作系统时钟源
#define         SYS_CLOCK_INPUT_RC_250k     0
#define         SYS_CLOCK_INPUT_PAT         1
#define         SYS_CLOCK_INPUT_RTC_OSC     2
#define         SYS_CLOCK_INPUT_RC_16M      3
#define         SYS_CLOCK_INPUT_BT_OSC      4          //BTOSC 双脚(12-26M)
#define         SYS_CLOCK_INPUT_BT_OSC_X2   5       //BTOSC 双脚(12-26M)

///衍生时钟源作系统时钟源
#define         SYS_CLOCK_INPUT_PLL_LRC     6
#define         SYS_CLOCK_INPUT_PLL_RC_16M  7
#define         SYS_CLOCK_INPUT_PLL_BT_OSC  8
#define         SYS_CLOCK_INPUT_PLL_PAT     9
#define         SYS_CLOCK_INPUT_PLL_RCL     10

typedef int SYS_CLOCK_INPUT;

/**
 * @brief clk_early_init 时钟初始化
 *
 * @param sys_in SYS_CLOCK_INPUT_BT_OSC SYS_CLOCK_INPUT_PLL_RCL
 * @param input_freq 24000000
 * @param out_freq sys 时钟
 *
 * @return
 */
int clk_early_init(u8 sys_in, u32 input_freq, u32 out_freq);

typedef enum {
    SYS_ICLOCK_INPUT_BTOSC,          //BTOSC 双脚(12-26M)
    SYS_ICLOCK_INPUT_BTOSC_X2,          //BTOSC 双脚(12-26M)
    SYS_ICLOCK_INPUT_STD24M,
    SYS_ICLOCK_INPUT_RTC_OSC,
    SYS_ICLOCK_INPUT_LRC,
    SYS_ICLOCK_INPUT_PAT,
} SYS_ICLOCK_INPUT;

typedef enum {
    PA0_CLOCK_OUTPUT = 0,
    PA0_CLOCK_OUT_BT_OSC,
    PA0_CLOCK_OUT_RTOSCH,
    PA0_CLOCK_OUT_NULL,

    PA0_CLOCK_OUT_LSB = 4,
    PA0_CLOCK_OUT_HSB,
    PA0_CLOCK_OUT_SFC,
    PA0_CLOCK_OUT_PLL,
} PA0_CLK_OUT;

typedef enum {
    PB8_CLOCK_OUTPUT = 0,
    PB8_CLOCK_OUT_RC,
    PB8_CLOCK_OUT_LRC,
    PB8_CLOCK_OUT_NULL,

    PB8_CLOCK_OUT_PLL75M = 4,
    PB8_CLOCK_OUT_XOSC_FSCK,
    PB8_CLOCK_OUT_PLL320,
    PB8_CLOCK_OUT_PLL107,
} PB8_CLK_OUT;

typedef enum {
    CLK_DIV_1,
    CLK_DIV_4,
    CLK_DIV_16,
    CLK_DIV_64,
    CLK_DIV_2,
    CLK_DIV_8,
    CLK_DIV_32,
    CLK_DIV_128,
    CLK_DIV_256,
    CLK_DIV_1024,
    CLK_DIV_4096,
    CLK_DIV_16384,
    CLK_DIV_512,
    CLK_DIV_2048,
    CLK_DIV_8192,
    CLK_DIV_32768,
} CLK_DIV_4bit;

/*
 * system enter critical and exit critical handle
 * */
struct clock_critical_handler {
    void (*enter)();
    void (*exit)();
};

#define CLOCK_CRITICAL_HANDLE_REG(name, enter, exit) \
	const struct clock_critical_handler clock_##name \
		 SEC_USED(.clock_critical_txt) = {enter, exit};

extern struct clock_critical_handler clock_critical_handler_begin[];
extern struct clock_critical_handler clock_critical_handler_end[];

#define list_for_each_loop_clock_critical(h) \
	for (h=clock_critical_handler_begin; h<clock_critical_handler_end; h++)


/**
 * @brief clk_get 获取各种模块的时钟源
 *
 * @param name
 * @todo name 包含 sys lsb uart timer sd spi adc mcpwm plink pll
 *
 * @return
 */
int clk_get(const char *name);


/**
 * @brief clk_set_en 0 关闭时钟切换功能，1；允许调用clk_set 修改系统时钟
 *
 * @param en
 */
void clk_set_en(u8 en);

/**
 * @brief clk_set 支持设置 sys 时钟
 *
 * @param name
 * @param clk [24|26|30|32|34|40|48|60|68|80|96|120|160] * 100000
 *
 * @return 返回实际频率
 */
int clk_set(const char *name, int clk);


enum CLK_OUT_SOURCE {
    NONE = 0,
    SFC_CLK_OUT,
    HSB_CLK_OUT,
    LSB_CLK_OUT,
    STD_48M_CLK_OUT,
    STD_24M_CLK_OUT,
    RC16M_CLK_OUT,
    LRC_CLK_OUT,
    RTC_OSC_CLK_OUT,
    BTOSC_24M_CLK_OUT,
    BTOSC_48M_CLK_OUT,
    XOSC_FSCL_CLK_OUT,
    P33_RCLK_CLK_OUT,
    PLL_ALINK0_CLK_OUT,
    PLL_D4P5_CLK_OUT,
    PLL_75M_CLK_OUT,
};

/**
 * @brief clk_out 输出时钟到io
 *
 * @param gpio
 * @param clk
 */
void clk_out(u8 gpio, enum CLK_OUT_SOURCE clk);

/**
 * @brief clk_dump 打印时钟配置信息
 */
void clk_dump(void);



#define MHz	(1000000L)

enum clk_mode {
    CLOCK_MODE_ADAPTIVE = 0,///< 自适应模式，系统跟进当前跑的频率，自动调整sys_dvdd 电压
    CLOCK_MODE_USR,         ///< 自定义sys_dvdd(内核电压)等级
};

/* ***************************************************************************/
/**
 * \Brief :         频率电压适配模式接口，支持动态配置频率电压为自适应或用户设置
 *
 * \Param :         mode    : CLOCK_MODE_ADAPTIVE 频率电压自适应使能 / CLOCK_MODE_USR 频率电压用户控制
 * \Param :         sys_dvdd: 用户设置值
 */
/* *****************************************************************************/
void clk_voltage_mode(enum clk_mode mode, u8 sys_dvdd);

/**
 * @brief clk_set_sfc_max_freq
 * 使用前需要保证所使用的flash支持4bit 100Mhz 模式
 *
 * @param dual_max_freq for cmd 3BH BBH
 * @param quad_max_freq for cmd 6BH EBH
 */
void clk_set_sfc_max_freq(u32 dual_max_freq, u32 quad_max_freq);


void clk_update_voltage_by_freq(u8 charge_en);

#endif

