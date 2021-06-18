#include "asm/includes.h"
#include "typedef.h"
#include "printf.h"
#include "chargestore.h"
#include "asm/spi.h"
#include "asm/adc_api.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "vm_id.h"

#define CONFIG_DEBUG_ENABLE     1
#define TCFG_CHARGE_ENABLE      1
#define TCFG_TEST_BOX_ENABLE    1
#define TCFG_LOWPOWER_LOWPOWER_SEL 1
#define TCFG_LOWPOWER_POWER_SEL				PWR_LDO15//电源模式设置，可选DCDC和LDO

#define TCFG_CLOCK_MODE     CLOCK_MODE_ADAPTIVE

#define ENABLE								1
#define DISABLE								0
#define TCFG_HW_SPI1_ENABLE     1
#define TCFG_HW_SPI2_ENABLE     1
#define TCFG_HW_IIC_ENABLE      1
#define TCFG_SOFT_IIC_ENABLE    1

#define TCFG_CLOCK_OSC_HZ       24000000
#define TCFG_CLOCK_SYS_HZ       24000000

/************************** debug uart config ****************************/
const struct uart_platform_data debug_uart_config = {
    .tx_pin = IO_PORTA_05,
    .rx_pin = 0xff,
    .baudrate = 1000000,
};

/************************** SPI config ****************************/
#if TCFG_HW_SPI1_ENABLE
const struct spi_platform_data spi1_p_data = {
    .port = {
        IO_PORTB_06,    //CLK
        IO_PORTB_07,    //DO
        IO_PORTB_08,    //DI
    },
    .mode = SPI_MODE_BIDIR_1BIT,
    .clk = 1000000,
    .role = SPI_ROLE_MASTER,
};
#endif
#if TCFG_HW_SPI2_ENABLE
const struct spi_platform_data spi2_p_data = {
    .port = {
        IO_PORTB_01,    //CLK
        IO_PORTB_02,    //DO
        IO_PORTB_03,    //DI
    },
    .mode = SPI_MODE_BIDIR_1BIT,
    .clk = 1000000,
    .role = SPI_ROLE_SLAVE,
};
#endif
/************************** IIC config ****************************/
#if TCFG_HW_IIC_ENABLE
const struct hw_iic_config hw_iic_cfg[] = {
    //iic0 data
    {
        //         SCL          SDA
        .port = {IO_PORTC_04, IO_PORTC_05},
        .baudrate = 100000,      //IIC通讯波特率
        .hdrive = 0,             //是否打开IO口强驱
        .io_filter = 1,          //是否打开滤波器（去纹波）
        .io_pu = 1,              //是否打开上拉电阻，如果外部电路没有焊接上拉电阻需要置1
        .role = IIC_MASTER,
    },
};
#endif
#if TCFG_SOFT_IIC_ENABLE
const struct soft_iic_config soft_iic_cfg[] = {
    {
        //iic0 data
        .scl = IO_PORTC_02,      //IIC CLK脚
        .sda = IO_PORTC_03,      //IIC DAT脚
        .delay = 50,             //软件IIC延时参数，影响通讯时钟频率
        .io_pu = 1,              //是否打开上拉电阻，如果外部电路没有焊接上拉电阻需要置1
    },
#if 0
    {
        //iic1 data
        .scl = IO_PORTA_06,
        .sda = IO_PORTA_07,
        .delay = 50,
        .io_pu = 1,
    },
#endif
};
#endif

/************************** CHARGE config****************************/
#if TCFG_CHARGE_ENABLE
struct charge_platform_data charge_data = {
    .charge_full_V          = CHARGE_FULL_V_4222,               //充电截止电压
    .charge_full_mA			= CHARGE_FULL_mA_10,                //充电截止电流
    .charge_mA				= CHARGE_mA_50,                     //充电电流
    /*充满判定过滤值，过滤时间 = (filter*2)ms,(WPWR>VBAT)&充满信号且时间大于过滤时间才认为充满*/
    .charge_full_filter     = 5,
    /*vpwr拔出过滤值，过滤时间 = (filter*2)ms,VPWR<0.6V且时间大于过滤时间才认为拔出*/
    .vpwr_off_filter		= 100,
    /*vpwr插入过滤值，过滤时间 = (filter*2)ms,WPWR>VBAT且时间大于过滤时间才认为插入*/
    .vpwr_on_filter         = 50,
    /*vpwr维持电压过滤值，过滤时间 = (filter*2)ms,0.6<WPWR<VBAT且时间大于过滤时间才认为是维持电压*/
    .vpwr_keep_filter       = 220,
    .vpwr_pulldown_lvl      = CHARGE_PULLDOWN_200K,             //下拉电阻档位选择
    .vpwr_pulldown_keep     = 1,                                //软关机是否要保持下拉
    .vpwr_pulldown_en		= 1,                                //下拉电阻使能位
    .pwr_sel                = TCFG_LOWPOWER_POWER_SEL,          //电源模式
};
#endif//TCFG_CHARGE_ENABLE

/************************** chargestore config****************************/
#if TCFG_TEST_BOX_ENABLE
static const struct chargestore_platform_data chargestore_data = {
    .baudrate               = 9600,
    .io_port                = IO_PORTP_00,
    .init                   = chargestore_init,
    .open                   = chargestore_open,
    .close                  = chargestore_close,
    .write                  = chargestore_write,
    .set_callback           = chargestore_set_data_callback,
    .set_update             = chargestore_set_update_ram,
};
#endif


/************************** key driver config****************************/
#if (USE_KEY_DRIVER == 1)
//2个测试io按键
u16 io_key_table[2] = {
    IO_PORTA_01,
    IO_PORTA_02,
};
#endif


void debug_uart_early_init()
{
#if CONFIG_DEBUG_ENABLE
    debug_uart_init(&debug_uart_config);
#endif
}

/************************** LOW POWER config ****************************/
const struct low_power_param power_param = {
    .config         = TCFG_LOWPOWER_LOWPOWER_SEL,          //0：sniff时芯片不进入低功耗  1：sniff时芯片进入powerdown
    .btosc_hz       = TCFG_CLOCK_OSC_HZ,                   //外接晶振频率
    .delay_us       = TCFG_CLOCK_SYS_HZ / 1000000L,        //提供给低功耗模块的延时(不需要需修改)
    .btosc_disable  = 0,        						   //进入低功耗时BTOSC是否保持
    .vddiom_lev     = VDDIOM_VOL_30V,          //强VDDIO等级,可选：2.0V  2.2V  2.4V  2.6V  2.8V  3.0V  3.2V  3.6V
    .vddiow_lev     = VDDIOW_VOL_28V,          //弱VDDIO等级,可选：2.1V  2.4V  2.8V  3.2V
    .osc_type       = OSC_TYPE_LRC,
    .lpctmu_en 		= 0,
    .vddio_keep     = 0,
};

/************************** PWR config ****************************/
struct port_wakeup port0 = {
    .pullup_down_enable = ENABLE,                            //配置I/O 内部上下拉是否使能
    .edge               = FALLING_EDGE,                      //唤醒方式选择,可选：上升沿\下降沿
    .both_edge          = 0,
    .filter             = PORT_FLT_8ms,
    .iomap              = IO_PORTB_01,                       //唤醒口选择
};

#if TCFG_CHARGE_ENABLE
struct port_wakeup charge_port = {
    .edge               = RISING_EDGE,                       //唤醒方式选择,可选：上升沿\下降沿
    .both_edge          = 0,
    .filter             = PORT_FLT_16ms,
    .iomap              = IO_CHGFL_DET,                      //唤醒口选择
};

struct port_wakeup vbat_port = {
    .edge               = RISING_EDGE,                      //唤醒方式选择,可选：上升沿\下降沿
    .both_edge          = 0,
    .filter             = PORT_FLT_16ms,
    .iomap              = IO_VBTCH_DET,                      //唤醒口选择
};

struct port_wakeup vpwr_rise_port = {
    .edge               = RISING_EDGE,                       //唤醒方式选择,可选：上升沿\下降沿
    .both_edge          = 1,
    .filter             = PORT_FLT_2ms,
    .iomap              = IO_LDOIN_DET,                      //唤醒口选择
};
#endif

#if TCFG_CHARGE_ENABLE || TCFG_TEST_BOX_ENABLE
struct port_wakeup vpwr_fall_port = {
    .edge               = FALLING_EDGE,                      //唤醒方式选择,可选：上升沿\下降沿
    .both_edge          = 1,
    .filter             = PORT_FLT_2ms,
    .iomap              = IO_LDOIN_DET,                      //唤醒口选择
};
#endif

const struct wakeup_param wk_param = {
#if (!TCFG_LP_TOUCH_KEY_ENABLE)
    .port[1] = &port0,
#endif
#if TCFG_CHARGE_ENABLE
    .port[2] = &charge_port,
    .port[3] = &vbat_port,
    .port[4] = &vpwr_rise_port,
#endif
#if TCFG_CHARGE_ENABLE || TCFG_TEST_BOX_ENABLE
    .port[5] = &vpwr_fall_port,
#endif
};


//maskrom 使用到的io
static void mask_io_cfg()
{
    struct boot_soft_flag_t boot_soft_flag = {0};
    boot_soft_flag.flag0.boot_ctrl.wdt_dis = 0;
    boot_soft_flag.flag0.boot_ctrl.poweroff = 0;
    boot_soft_flag.flag0.boot_ctrl.is_port_b = JL_IOMAP->CON0 & BIT(16) ? 1 : 0;

    boot_soft_flag.flag1.misc.usbdm = SOFTFLAG_HIGH_RESISTANCE;
    boot_soft_flag.flag1.misc.usbdp = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag1.misc.uart_key_port = 0;
    boot_soft_flag.flag1.misc.ldoin = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag2.pa7_pb4.pa7 = SOFTFLAG_HIGH_RESISTANCE;
    boot_soft_flag.flag2.pa7_pb4.pb4 = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag3.pc3_pc5.pc3 = SOFTFLAG_HIGH_RESISTANCE; //touch key power support
    boot_soft_flag.flag3.pc3_pc5.pc5 = SOFTFLAG_HIGH_RESISTANCE;
    mask_softflag_config(&boot_soft_flag);
}

/*进软关机之前默认将IO口都设置成高阻状态，需要保留原来状态的请修改该函数*/
extern void dac_power_off(void);

int gpio_set_pu(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);
int gpio_set_pd(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);
int gpio_die(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);
int gpio_dieh(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);
int gpio_dir(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);
void board_set_soft_poweroff(void)
{
    u32 porta_value = 0xffff;
#if TCFG_LP_TOUCH_KEY_ENABLE
    u32 portb_value = 0xffff;
#else
    //保留长按Reset Pin - PB1
    u32 portb_value = 0xffff & (~BIT(1));
#endif /* #if TCFG_LP_TOUCH_KEY_ENABLE */
    u32 portc_value = 0xffff;

    mask_io_cfg();

    gpio_set_pu(GPIOA, 0, 16, ~porta_value, GPIO_AND);
    gpio_set_pd(GPIOA, 0, 16, ~porta_value, GPIO_AND);
    gpio_die(GPIOA, 0, 16, ~porta_value, GPIO_AND);
    gpio_dieh(GPIOA, 0, 16, ~porta_value, GPIO_AND);
    gpio_dir(GPIOA, 0, 16, porta_value, GPIO_OR);

    gpio_set_pu(GPIOB, 0, 16, ~portb_value, GPIO_AND);
    gpio_set_pd(GPIOB, 0, 16, ~portb_value, GPIO_AND);
    gpio_die(GPIOB, 0, 16, ~portb_value, GPIO_AND);
    gpio_dieh(GPIOB, 0, 16, ~portb_value, GPIO_AND);
    gpio_dir(GPIOB, 0, 16, portb_value, GPIO_OR);

    gpio_set_pu(GPIOC, 0, 16, ~portc_value, GPIO_AND);
    gpio_set_pd(GPIOC, 0, 16, ~portc_value, GPIO_AND);
    gpio_die(GPIOC, 0, 16, ~portc_value, GPIO_AND);
    gpio_dieh(GPIOC, 0, 16, ~portc_value, GPIO_AND);
    gpio_dir(GPIOC, 0, 16, portc_value, GPIO_OR);

    gpio_set_pull_up(IO_PORTP_00, 0);
    gpio_set_pull_down(IO_PORTP_00, 0);
    gpio_set_die(IO_PORTP_00, 0);
    gpio_set_dieh(IO_PORTP_00, 0);
    gpio_set_direction(IO_PORTP_00, 1);

    gpio_set_pull_up(IO_PORT_DP, 0);
    gpio_set_pull_down(IO_PORT_DP, 0);
    gpio_set_die(IO_PORT_DP, 0);
    gpio_set_dieh(IO_PORT_DP, 0);
    gpio_set_direction(IO_PORT_DP, 1);

    gpio_set_pull_up(IO_PORT_DM, 0);
    gpio_set_pull_down(IO_PORT_DM, 0);
    gpio_set_die(IO_PORT_DM, 0);
    gpio_set_dieh(IO_PORT_DM, 0);
    gpio_set_direction(IO_PORT_DM, 1);

    /*dac_power_off();*/
#if TCFG_CHARGE_ENABLE
    L5V_LOAD_EN(charge_get_vpwr_pulldown_en());
    L5V_RES_DET_S_SEL(charge_get_vpwr_pulldown_res());
#endif
}

#define     APP_IO_DEBUG_0(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT &= ~BIT(x);}
#define     APP_IO_DEBUG_1(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT |= BIT(x);}

void sleep_exit_callback(u32 usec)
{
    putchar('>');
    /*APP_IO_DEBUG_1(B, 0);*/
}

void sleep_enter_callback(u8  step)
{
    /* 此函数禁止添加打印 */
    if (step == 1) {
        putchar('<');
        /*APP_IO_DEBUG_0(B, 0);*/
        /*dac_power_off();*/
    } else {

        usb_iomode(1);

        gpio_set_pull_up(IO_PORT_DP, 0);
        gpio_set_pull_down(IO_PORT_DP, 0);
        gpio_set_direction(IO_PORT_DP, 1);
        gpio_set_die(IO_PORT_DP, 0);

        gpio_set_pull_up(IO_PORT_DM, 0);
        gpio_set_pull_down(IO_PORT_DM, 0);
        gpio_set_direction(IO_PORT_DM, 1);
        gpio_set_die(IO_PORT_DM, 0);
    }
}

static void wl_audio_clk_on(void)
{
    JL_WL_AUD->CON0 = 1;
}

u32 return_power_timeout(void)
{
    return -2;
}

void board_power_init(void)
{
    power_init(&power_param);

    power_set_callback(TCFG_LOWPOWER_LOWPOWER_SEL, sleep_enter_callback, sleep_exit_callback, board_set_soft_poweroff);

    wl_audio_clk_on();

    power_wakeup_init(&wk_param);
}

void board_init()
{
    adc_init();

    board_power_init();

    power_trim(VM_PMU_VOLTAGE, 0);

#if TCFG_CHARGE_ENABLE
    charge_init(&charge_data);
    clk_update_voltage_by_freq(TCFG_CHARGE_ENABLE);
#else
    CHGBG_EN(0);
    CHARGE_EN(0);
#endif

#if TCFG_TEST_BOX_ENABLE
    chargestore_api_init(&chargestore_data);
#endif

    clk_voltage_mode(TCFG_CLOCK_MODE, SYSVDD_VOL_SEL_126V);

    power_set_mode(TCFG_LOWPOWER_POWER_SEL);
}
