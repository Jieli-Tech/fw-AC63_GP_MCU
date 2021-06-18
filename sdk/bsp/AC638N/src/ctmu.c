#include "asm/ctmu.h"

//=================================================================================//
/*
ctmu原理:

  1.三个时钟源:
     1)闸门时钟源(可选), 分频值可以选
     2)放电时钟源(固定是lsb), 分频值可以选
     3)充电时钟源(可选), 分频值不可选

  2.闸门时间内
            _________________________________
         __|                                 |__
      		    __    __    __    __    __
 放电时计数  __|  |__|  |__|  |__|  |__|  |__

  3. 过程: 放电 --> 充电(计数) --> 放电[该过程可选]

  4.分析计数值
 */
//=================================================================================//

//时钟闸门时钟源选择, 可分频
enum {
    GATE_SOURCE_OSC = 0,
    GATE_SOURCE_LRC,
    GATE_SOURCE_PLL128M,
    GATE_SOURCE_PLL192M,
};

//时钟闸门时钟源分频选择
enum {
    GATE_SOURCE_PRE_DIV8 = 0,
    GATE_SOURCE_PRE_DIV16,
    GATE_SOURCE_PRE_DIV32,
    GATE_SOURCE_PRE_DIV64,
    GATE_SOURCE_PRE_DIV8192,
    GATE_SOURCE_PRE_DIV16384,
    GATE_SOURCE_PRE_DIV32768,
    GATE_SOURCE_PRE_DIV65536,
};

//放电时钟源硬件上默认是lsb, 可分频
enum {
    DISCHARGE_SOURCE_PRE_DIV1 = 0,
    DISCHARGE_SOURCE_PRE_DIV2,
    DISCHARGE_SOURCE_PRE_DIV4,
    DISCHARGE_SOURCE_PRE_DIV8,
    DISCHARGE_SOURCE_PRE_DIV16,
    DISCHARGE_SOURCE_PRE_DIV32,
    DISCHARGE_SOURCE_PRE_DIV64,
    DISCHARGE_SOURCE_PRE_DIV128,
};

//充电时钟源选择, 不可分频
enum {
    CHARGE_SOURCE_OSC = 0,
    CHARGE_SOURCE_LRC,
    CHARGE_SOURCE_PLL128M,
    CHARGE_SOURCE_PLL192M,
};

static const u8 ctmu_ch_io_table[8] = {
    IO_PORTA_03,
    IO_PORTB_07,
    IO_PORTA_05,
    IO_PORTA_06,
    IO_PORTC_02,
    IO_PORTC_03,
    IO_PORTB_04,
    IO_PORTB_06,
};

#define CTMU_CON0 		JL_CTM->CON0
#define CTMU_CON1 		JL_CTM->CON1
#define CTMU_ADR 		JL_CTM->ADR

#define CTMU_CH_MAX     8

static u32 ctm_buf[16 * 2] = {0};
static u8 port_index_mapping_talbe[CTMU_CH_MAX] = {0};

//简单的滤波算法需要
static u32 Touchkey_pre_value[CTMU_CH_MAX] = {0};
static u32 Touchkey_normal_value[CTMU_CH_MAX] = {0};
#define CALIBRATE_CYCLE 400//周期大约2秒
static u32 Touchkey_calibrate_cnt[CTMU_CH_MAX] = {0};
static u32 Touchkey_calibrate_tmp_value[CTMU_CH_MAX] = {0};


static u8 Touchkey_state = 0;

static const struct ctmu_touch_key_platform_data *__this = NULL;

/*
 * @brief 获取缓存buf里对应元素的值
 * @param ch_index :元素序号
 * @param ctmu_buf :缓存buf地址
 * @return 数组元素值，即触摸计数值
*/
static u32 get_ctmu_ch_val(u8 ch_index, u32 *ctm_buf)
{
    if (ch_index >= __this->num) {
        return 0;
    }
    return ctm_buf[port_index_mapping_talbe[ch_index]];
}

/*
 * @brief 简单的滤波，用户可重新写
 * @param ch: 通道号
 * @param value: 当前为滤波之前的值
 * @return 滤波后的值
*/
static u32 ctm_flt(u8 ch, u32 value)
{
    if (Touchkey_pre_value[ch] == 0) {
        Touchkey_pre_value[ch] = value;
    }
    if (value >= Touchkey_pre_value[ch]) {
        value = Touchkey_pre_value[ch] + ((value - Touchkey_pre_value[ch]) * 0.5f);
    } else {
        value = Touchkey_pre_value[ch] - ((Touchkey_pre_value[ch] - value) * 0.2f);
    }
    Touchkey_pre_value[ch] = value;
    return value;
}

/*
 * @brief 中断里调用，处理触摸计数值函数
 * @param ch_index: 通道号
 * @param ctmu_buf: 当前使用的缓存buf地址
*/
static void scan_capkey(u8 ch_index, u32 *ctmu_buf)
{
    if (__this == NULL || __this->num == 0) {
        return;
    }
    u32 Touchkey_cur_value = get_ctmu_ch_val(ch_index, ctmu_buf);       //当前采到的值
    /* printf("ch: %d  val: %d\n", ch_index, Touchkey_cur_value); */
    u32 Touchkey_flt_value = ctm_flt(ch_index, Touchkey_cur_value);     //滤波后的值
    /* printf("ch: %d  val: %d\n", ch_index, Touchkey_flt_value); */

    if (Touchkey_flt_value > (Touchkey_normal_value[ch_index] + __this->port_list[ch_index].press_delta)) {
        Touchkey_state |=  BIT(ch_index);
        Touchkey_calibrate_cnt[ch_index] = 0;
    } else {
        Touchkey_state &= ~BIT(ch_index);
        Touchkey_calibrate_cnt[ch_index] ++;
    }
    //定期更新常态下的基准值
    if (Touchkey_calibrate_cnt[ch_index] > CALIBRATE_CYCLE) {
        Touchkey_normal_value[ch_index] = Touchkey_calibrate_tmp_value[ch_index] / 20;
        Touchkey_calibrate_tmp_value[ch_index] = 0;
        Touchkey_calibrate_cnt[ch_index] = 0;
    } else if (Touchkey_calibrate_cnt[ch_index] >= (CALIBRATE_CYCLE / 2)) {
        if (Touchkey_calibrate_cnt[ch_index] < ((CALIBRATE_CYCLE / 2) + 20)) {
            Touchkey_calibrate_tmp_value[ch_index] += Touchkey_flt_value;
        }
    } else {
        Touchkey_calibrate_tmp_value[ch_index] = 0;
    }

    /* if (ch_index == 0) { */
    /*     printf("%d  %d  %d\n", Touchkey_flt_value, Touchkey_normal_value[ch_index], __this->port_list[ch_index].press_delta); */
    /* } */
}

/*
 * @brief 中断函数
*/
___interrupt
static void ctmu_isr_handle(void)
{
    u32 *rbuf = NULL;
    if (CTMU_CON0 & BIT(7)) {
        CTMU_CON0 |= BIT(6);
    }
    if (CTMU_CON0 & BIT(9)) {
        rbuf = (u32 *)(&ctm_buf[0]);
    } else {
        rbuf = (u32 *)(&ctm_buf[16]);
    }
    for (u8 i = 0; i < __this->num; i++) {
        scan_capkey(i, rbuf);
    }
}
/*
 * @brief ctmu io初始化函数
 * @param port_list: 带引脚信息的结构体指针
 * @param port_num: 引脚数量
*/
static void ctmu_port_init(const struct ctmu_key_port *port_list, u8 port_num)
{
    u8 i, j;
    for (i = 0; i < port_num; i++) {
        j = port_list[i].ch_io;
        if (j < 8) {
            CTMU_CON1 |= BIT(j);
            port_index_mapping_talbe[i] = j;
            j = ctmu_ch_io_table[j];
            gpio_set_pull_down(j, 0);
            gpio_set_pull_up(j, 0);
            gpio_set_die(j, 0);
            gpio_set_direction(j, 1);
        } else {
            printf("ctmu io ch num err!\n");
        }
    }
}

/*
 * @brief ctmu 缓存buf地址赋给模块的DMA
*/
static void ctmu_buf_init(void)
{
    memset((u8 *)&ctm_buf, 0x00, sizeof(ctm_buf));
    CTMU_ADR = (u32)&ctm_buf;
}

/*
 * @brief ctmu 模块的初始化函数
 * @param port_list: 带引脚信息的结构体指针
 * @param num: 引脚数量
*/
static void touch_ctmu_init(const struct ctmu_key_port *port, u8 num)
{
    CTMU_CON0 = 0;
    CTMU_CON1 = 0;
    ctmu_port_init(port, num);
    ctmu_buf_init();
    //充电时钟选择
    CTMU_CON0 |= (CHARGE_SOURCE_PLL192M << 10);
    //放电时钟分频选择, 时钟源固定是lsb
    CTMU_CON0 |= (DISCHARGE_SOURCE_PRE_DIV128 << 12); //要求 > 2uS
    //闸门时钟选择
    CTMU_CON0 |= (GATE_SOURCE_PLL192M << 4); //192 MHz
    CTMU_CON0 |= (GATE_SOURCE_PRE_DIV65536 << 1); //0.34ms
    //充电电流
    CTMU_CON0 |= 3 << 18;  //0 ~ 7
    //comparator reference voltage
    CTMU_CON0 |= 2 << 16;  //0 ~ 3
    //放电模式(在充电完成后是否放电, 这样会放电更彻底)
    CTMU_CON0 |= 1 << 23;
    CTMU_CON0 |= 1 << 6;  // Clear  Pending
}

/*
 * @brief ctmu 模块使能函数
 * @param en:  0,关闭   1,打开
*/
static void touch_ctmu_enable(u8 en)
{
    if (en) {
        CTMU_CON0 |= BIT(8); //Int Enable
        CTMU_CON0 |= BIT(0); //Moudle Enable
    } else {
        CTMU_CON0 &= ~BIT(8); //Int Disable
        CTMU_CON0 &= ~BIT(0); //Moudle Disable
    }
}

/*
 * @brief  ctmu 总初始化函数
 * @param pdata 初始化的参数结构体地址 ： struct ctmu_touch_key_platform_data *
 * @return  0,成功  非0,失败
*/
int ctmu_init(void *pdata)
{
    __this = (const struct ctmu_touch_key_platform_data *)pdata;
    if (!__this) {
        return -1;
    }
    if (__this->num > CTMU_CH_MAX) {
        printf("ctmu num err!!!");
        return -1;
    }
    touch_ctmu_init(__this->port_list, __this->num);
    request_irq(IRQ_CTM_IDX, 1, ctmu_isr_handle, 0);
    for (u8 i = 0; i < __this->num; i++) {
        Touchkey_normal_value[i] = 0 - (2 * __this->port_list[i].press_delta);
    }
    touch_ctmu_enable(1);
    return 0;
}

/*
 * @brief 获取注册的触摸按键的序号
 * @return  按键序号
*/
u8 get_ctmu_value(void)
{
    static u8 pre_i = 0;
    if (Touchkey_state & BIT(pre_i)) {
        /* printf("i:%d\n", pre_i); */
        return pre_i;
    }
    for (u8 i = 0; i < __this->num; i++) {
        if (Touchkey_state & BIT(i)) {
            /* printf("i:%d\n", i); */
            pre_i = i;
            return i;
        }
    }
    return 0xff;
}


/************************************************ 以下是SDK提供的参考示例 ***************************************************/
// *INDENT-OFF*
static const struct ctmu_key_port test_touch_ctmu_port[] = {
    //按键0
    {
        .press_delta = 10000,   //变化量的阈值
        .ch_io = CTMU_CH_PC02,  //通道号
        .key_value = 0x12,      //预留的按键值
    },
    //按键1
    {
        .press_delta = 10000,   //变化量的阈值
        .ch_io = CTMU_CH_PB06,  //通道号
        .key_value = 0x34,      //预留的按键值
    },
};
CTMU_PLATFORM_DATA_BEGIN(test_ctmu_pdata)
    .num = ARRAY_SIZE(test_touch_ctmu_port),
    .port_list = test_touch_ctmu_port,
CTMU_PLATFORM_DATA_END()

void ctmu_test(void)
{
    printf("*****************  ctmu test  ***************\n");
    ctmu_init((void *)&test_ctmu_pdata);

    u8 key_num = 0;
    extern void wdt_clr();
    while (1) {
        wdt_clr();
        key_num = get_ctmu_value();
        if (key_num != 0xff) {
            printf("key_num = %d\n", key_num);
        }
        delay(100000);
    }
}


