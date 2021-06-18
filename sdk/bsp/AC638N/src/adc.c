#include "asm/adc_api.h"



/*
 * @brief PMU通道的子通道的选择
 * @param ch : 子通道号
*/
static void adc_pmu_ch_sel(u32 ch)
{
    P33_CON_SET(P3_ANA_CON4, 1, 4, ch);
}

/*
 * @brief PMU内部通道开关
*/
static void adc_pmu_ch_en(u8 en)
{
    if (en) {
        P33_CON_SET(P3_ANA_CON4, 0, 1, 1);
        P33_CON_SET(P3_ANA_CON4, 5, 1, 1);
        P33_CON_SET(P3_ANA_CON4, 6, 1, 1);
        P33_CON_SET(P3_ANA_CON13, 5, 1, 1);
    } else {
        P33_CON_SET(P3_ANA_CON4, 0, 1, 0);
    }
}

/*
 * @brief 中断函数
*/
static void (*adc_isr_cbfun[14])(u8 ch, u16 adc_val) = {NULL};
___interrupt
static void adc_isr(void)
{
    u16 adc_val = JL_ADC->RES;
    JL_ADC->CON &= ~BIT(5);//dis ie
    JL_ADC->CON |=  BIT(6);
    u8 ch = (JL_ADC->CON >> 8) & 0xf;
    if (adc_isr_cbfun[ch]) {
        adc_isr_cbfun[ch](ch, adc_val);
    }
}

/*
 * @brief 采用异步的方式采值，使能采值后就走，不死等，通过注册中断回调函数，等一下转换结束会起中断，中断里执行回调函数
 * @param ch : ADC通道号
 * @param cbfun : 注册的中断回调函数
*/
void adc_get_value_by_isr(u32 ch, void (*cbfun)(u8, u16))
{
    adc_isr_cbfun[ch & 0xf] = cbfun;
    if ((ch & 0xf) == AD_CH_PMU) {
        adc_pmu_ch_sel(ch >> 16);
    }
    SFR(JL_ADC->CON, 8, 4, ch & 0xf);
    JL_ADC->CON |=  BIT(5);//en ie
    JL_ADC->CON |=  BIT(6);//start
}

/*
 * @brief 采用等待的方式采值，直到转换结束，才出来，即死等
 * @param ch : ADC通道号
 * @return 当前通道的AD值
*/
u32 adc_get_value(u32 ch)
{
    if ((ch & 0xf) == AD_CH_PMU) {
        adc_pmu_ch_sel(ch >> 16);
    }
    SFR(JL_ADC->CON, 8, 4, ch & 0xf);
    JL_ADC->CON &= ~BIT(5);//dis ie
    JL_ADC->CON |=  BIT(6);//start
    while (!(JL_ADC->CON & BIT(7)));
    return JL_ADC->RES;
}


/*
 * @brief 换算电压的公式函数
 * @param adc_vbg : 基准通道的AD值, 即AD_CH_LDOREF
 * @param adc_ch_val : 当前通道的AD值
 * @return 当前通道的电压值，单位mv
*/
#define VBG_CENTER  801
#define VBG_RES     3
u32 adc_value_to_voltage(u32 adc_vbg, u32 adc_ch_val)
{
    u32 adc_res = adc_ch_val;
    u32 adc_trim = get_vbg_trim();
    u32 tmp, tmp1;
    tmp1 = adc_trim & 0x0f;
    tmp = (adc_trim & BIT(4)) ? VBG_CENTER - tmp1 * VBG_RES : VBG_CENTER + tmp1 * VBG_RES;
    adc_res = adc_res * tmp / adc_vbg;
    return adc_res;
}

/*
 * @brief 获取通道是电压值
 * @param ch : ADC通道号
 * @return 当前通道的电压值，单位mv
*/
u32 adc_get_voltage(u32 ch)
{
    u32 adc_vbg = adc_get_value(AD_CH_LDOREF);
    u32 adc_res = adc_get_value(ch);
    return adc_value_to_voltage(adc_vbg, adc_res);
}

/*
 * @brief adc模块的初始化函数
*/
void adc_init(void)
{
    JL_ADC->CON = BIT(6);
    JL_ADC->CON &= ~(0b111 < 0);
    JL_ADC->CON |= (0b110 < 0);     //div 96
    JL_ADC->CON &= ~(0b1111 < 12);
    JL_ADC->CON |= (0b1111 < 12);   //15 * 8clk
    JL_ADC->CON |=  BIT(3) | BIT(4);//ADC EN
    request_irq(IRQ_SARADC_IDX, 0, adc_isr, 0);
    adc_pmu_ch_en(1);
}


/***************************  以下是SDK提供的参考示例  ************************************/

static u16 _vbg_val = 0;
static void user_adc_cbfun(u8 ch, u16 adc_val)
{
    u32 tmp_vol = adc_value_to_voltage(_vbg_val, adc_val);
    printf("ch = %d, adc = %d  -->  %d mv\n", ch, adc_val, tmp_vol);
}
void adc_test(void)
{
    printf("*****************  adc test  ***************\n");
    adc_init();

    u32 adc_io = IO_PORTA_03;
    gpio_set_die(adc_io, 0);        //将io设为模拟功能
    //浮空输入
    gpio_set_direction(adc_io, 1);  //方向设为输入
    gpio_set_pull_up(adc_io, 0);    //关上拉10K
    gpio_set_pull_down(adc_io, 0);  //关下拉10K

    adc_io = IO_PORTC_04;
    gpio_set_die(adc_io, 0);        //将io设为模拟功能
    //浮空输入
    gpio_set_direction(adc_io, 1);  //方向设为输入
    gpio_set_pull_up(adc_io, 0);    //关上拉10K
    gpio_set_pull_down(adc_io, 0);  //关下拉10K

    u16 adc_val = 0;
    u16 io_vol = 0;
    extern void wdt_clr();
    while (1) {
        wdt_clr();
        delay(1000000);
        _vbg_val = adc_get_value(AD_CH_LDOREF);
        adc_val = adc_get_value(AD_CH_PA3);
        io_vol = adc_get_voltage(AD_CH_PA3);
        adc_get_value_by_isr(AD_CH_PC4, user_adc_cbfun);
        printf("adc_val = %d  >>>  %d mv\n", adc_val, io_vol);
    }
}

