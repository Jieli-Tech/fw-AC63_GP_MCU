#ifndef __ADC_API_H__
#define __ADC_API_H__

#include "typedef.h"
#include "asm/includes.h"

//AD channel define
#define AD_CH_PA3    (0x0)
#define AD_CH_PA5    (0x1)
#define AD_CH_PA6    (0x2)
#define AD_CH_PC3    (0x3)
#define AD_CH_PC4    (0x4)
#define AD_CH_PC5    (0x5)
#define AD_CH_PB1    (0x6)
#define AD_CH_PB2    (0x7)
#define AD_CH_PB7    (0x8)
#define AD_CH_PB6    (0x9)
#define AD_CH_DP     (0xa)
#define AD_CH_DM     (0xb)
#define AD_CH_PB4    (0xc)
#define AD_CH_PMU    (0xd)


#define ADC_PMU_CH_VBG       (0x0<<16)  //MVBG
#define ADC_PMU_CH_VDC13     (0x1<<16)
#define ADC_PMU_CH_SYSVDD    (0x2<<16)
#define ADC_PMU_CH_DTEMP     (0x3<<16)
#define ADC_PMU_CH_PROGF     (0x4<<16)
#define ADC_PMU_CH_VBAT      (0x5<<16)     //1/4vbat
#define ADC_PMU_CH_LDO5V     (0x6<<16)     //1/4 LDO5V
#define ADC_PMU_CH_WVDD      (0x7<<16)
#define ADC_PMU_CH_PVDD      (0x8<<16)
#define ADC_PMU_CH_RVDD      (0x9<<16)
#define ADC_PMU_CH_VSW       (0xa<<16)
#define ADC_PMU_CH_PROGI     (0xb<<16)
#define ADC_PMU_CH_EVDD      (0xc<<16)
#define ADC_PMU_CH_VBGW08    (0xd<<16)  //WVBG


#define AD_CH_PMU_VBG   (AD_CH_PMU | ADC_PMU_CH_VBG)
#define AD_CH_VDC13     (AD_CH_PMU | ADC_PMU_CH_VDC13)
#define AD_CH_SYSVDD    (AD_CH_PMU | ADC_PMU_CH_SYSVDD)
#define AD_CH_DTEMP     (AD_CH_PMU | ADC_PMU_CH_DTEMP)
#define AD_CH_VBAT      (AD_CH_PMU | ADC_PMU_CH_VBAT)
#define AD_CH_LDO5V     (AD_CH_PMU | ADC_PMU_CH_LDO5V)
#define AD_CH_WVDD      (AD_CH_PMU | ADC_PMU_CH_WVDD)
#define AD_CH_PVDD      (AD_CH_PMU | ADC_PMU_CH_PVDD)

#define AD_CH_LDOREF    AD_CH_PMU_VBG


/*
 * @brief 采用异步的方式采值，使能采值后就走，不死等，通过注册中断回调函数，等一下转换结束会起中断，中断里执行回调函数
 * @param ch : ADC通道号
 * @param cbfun : 注册的中断回调函数
*/
void adc_get_value_by_isr(u32 ch, void (*cbfun)(u8, u16));

/*
 * @brief 采用等待的方式采值，直到转换结束，才出来，即死等
 * @param ch : ADC通道号
 * @return 当前通道的AD值
*/
u32 adc_get_value(u32 ch);

/*
 * @brief 换算电压的公式函数
 * @param adc_vbg : 基准通道的AD值, 即AD_CH_LDOREF
 * @param adc_ch_val : 当前通道的AD值
 * @return 当前通道的电压值，单位mv
*/
u32 adc_value_to_voltage(u32 adc_vbg, u32 adc_ch_val);

/*
 * @brief 获取通道的电压值
 * @param ch : ADC通道号
 * @return 当前通道的电压值，单位mv
*/
u32 adc_get_voltage(u32 ch);

/*
 * @brief adc模块的初始化函数
*/
void adc_init(void);

/*
 * @brief SDK提供的参考函数
*/
void adc_test(void);



#endif

