#ifndef _PWM_LED_H_
#define _PWM_LED_H_

#include "typedef.h"
#include "asm/includes.h"

/****************************************************************************************************************

使用说明 ：（必看）

    通常我们对PWM的认知是：就一种PWM波形，在一个周期内，由高电平和低电平组成，它们所占时间比例可调。
    但是，LED_PWM不是这样的，他有两种PWM波形，
    第一种PWM波形是：在一个周期内，由高电平和高阻态组成，它们所占时间比例可调。在此命名为 h_pwm，h_pwm用于控制LED高电平亮灯的环境
    第二种PWM波形是：在一个周期内，由低电平和高阻态组成，它们所占时间比例可调。在此命名为 l_pwm，l_pwm用于控制LED低电平亮灯的环境

    注意：高阻态可能是不定的电平，它的电平由外部电路决定，总之高阻态不会对外围电路起作用。比如接led灯，高阻态不会使灯亮的

    时钟源： PWM_LED模块的时钟源只有一个：LRC时钟源，大概为32KHz，所以PWM_LED模块的最小时间单位是 (1 / 32000) 秒，

    PWM_LED模块内部有个独立计数器，计数器计数从0开始自加，计到某个计数值时就又从0开始计。即完成了一个计数器周期。

    固定波形输出模式：
    PWM_LED模块是以计数器周期来管理h_pwm和l_pwm的输出，意思是在当前的计数周期内输出的是h_pwm，而下一个计数器周期就不一定输出h_pwm了，可能会输出l_pwm。
    固定波形输出模式的内部独立计数器的计数最大值是255，意思是计数计到255后，又从0开始计。
    所以可以通参数过配置出几种效果：
        1. 每个计数周期都输出h_pwm。
        2. 每个计数周期都输出l_pwm。
        3. 前一个计数周期输出l_pwm，后一个计数周期输出l_pwm，这样每过一个计数周期就会切换输出，.....->h_pwm->l_pwm->h_pwm->l_pwm->.....
        4. 前两个计数周期输出l_pwm，后两个计数周期输出l_pwm，这样每过两个计数周期就会切换输出，.....->h_pwm->h_pwm->l_pwm->l_pwm->h_pwm->h_pwm->l_pwm->l_pwm->......
        5. 前 n个计数周期输出l_pwm，后 n个计数周期输出l_pwm，这样每过n 个计数周期就会切换输出

    呼吸变化输出模式：
    指的是pwm的占空比会先递增再递减的变化，占空比从0开始，每过一个PWM周期就会自加1，当加到设置的最大占空比时，就不会自加了，而准备开始自减1，直至减到0。
    所以占空比递增的过程的时间 = pwm周期 * 最大占空比对应的计数值。同样，占空比递减的过程的时间 = pwm周期 * 最大占空比对应的计数值。
    注意：当占空比加到大值的时候，可以维持一段时间再递减。这段时间可以配置，由内部的计数器计数定时实现。
    当占空比减到0的时候，也可以维持一段时间再递增。这段时间可以配置，由内部的计数器计数定时实现。

    故，pwm呼吸变化输出的周期 = 占空比递增的过程的时间 + 占空比加到大后维持的时间 + 占空比递减的过程的时间 + 占空比减到0后维持的时间；

******************************************************************************************************************/
// *INDENT-OFF*
struct pwm_led_platform_data {
    u8 port;    //输出波形的引脚
    u8 inv_prd; //范围0~7，0：关闭交替输出,即只有一种pwm输出，当inv_prd>0比如inv_prd=2; 那么输出两段h_pwm后，再输出两段l_pwm，这样循环交替：h_pwm->h_pwm->l_pwm->l_pwm->h_pwm->h_pwm->l_pwm->l_pwm->...
    u16 t_cnt_prd;//模块内部的一个独立的计数器,用于管理PWM输出的，其单位时间 ct = t_cnt_prd * (1 / 32000); 单位s，即每过ct那么长时间，内部计数器就会加1
    u16 pwm_prd;//h_pwm和l_pwm周期是一样的，pwm周期 = pwm_prd * (1 / 32000); 单位s
    u8 mode;    //0：固定波形模式   1：呼吸变化模式
    union {
        //固定波形输出模式
        struct {
            u16 h_pwm_duty; //h_pwm高电平的占空比，值不能大于pwm_prd，所以高电平占的时间 = pwm周期 * h_pwm_duty / pwm_prd; 单位s，则剩余时间为高阻态
            u16 l_pwm_duty; //l_pwm低电平的占空比，值不能大于pwm_prd，所以低电平占的时间 = pwm周期 * l_pwm_duty / pwm_prd; 单位s，则剩余时间为高阻态
            u8 t_cnt0;      //内部计数器在   0   ~ t_cnt0期间，模块会控制PWM引脚一直处于高阻状态，不允许PWM波形输出
            u8 t_cnt1;      //内部计数器在t_cnt0 ~ t_cnt1期间，模块会控制PWM引脚解除高阻态，允许PWM波形输出
            u8 t_cnt2;      //内部计数器在t_cnt1 ~ t_cnt2期间，模块会控制PWM引脚一直处于高阻状态，不允许PWM波形输出
            u8 t_cnt3;      //内部计数器在t_cnt2 ~ t_cnt3期间，模块会控制PWM引脚解除高阻态，允许PWM波形输出
                            //内部计数器在t_cnt3 ~   255 期间，模块会控制PWM引脚一直处于高阻状态，不允许PWM波形输出, 所以计数器周期 = 256 * ct = 256 * t_cnt_prd * (1 / 32000); 单位s
        } mode_0;

        //呼吸变化输出模式，即PWM的占空比是变化的
        struct {
            u16 h_pwm_duty_max;     //h_pwm高电平的最大占空比，值不能大于pwm_prd，即占空比慢慢递增加1，到的最大值，然后再慢慢递减到0
            u16 l_pwm_duty_max;     //l_pwm低电平的最大占空比，值不能大于pwm_prd，即占空比慢慢递增加1，到的最大值，然后再慢慢递减到0
            u8 h_pwm_duty_max_cnt;  //h_pwm占空比自加到max最大值后维持的时间 = h_pwm_duty_max_cnt * ct = h_pwm_duty_max_cnt * t_cnt_prd * (1 / 32000); 单位s
            u8 l_pwm_duty_max_cnt;  //l_pwm占空比自加到max最大值后维持的时间 = h_pwm_duty_max_cnt * ct = l_pwm_duty_max_cnt * t_cnt_prd * (1 / 32000); 单位s
            u16 pwm_duty_0_cnt;     //pwm占空比自增自减后，维持高组态的时间 = h_pwm_duty_max_cnt * ct = pwm_duty_0_cnt * t_cnt_prd * (1 / 32000); 单位s
        } mode_1;
    };
};

#define PWM_LED_PLATFORM_DATA_BEGIN(data) \
    struct pwm_led_platform_data data = {

#define PWM_LED_PLATFORM_DATA_END()  \
    };


/*
 * @brief  PWM_LED模块初始化函数
 * @arg pdata 初始化的参数结构体地址： struct pwm_led_platform_data *
*/
void pwm_led_init(void *pdata);

/*
 * @brief 参考示例
*/
void pwm_led_test(void);


#endif //_PWM_LED_H_


