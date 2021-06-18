#include "asm/pwm_led.h"


static struct pwm_led_platform_data *__this = NULL;

/*
 * @brief  PWM_LED模块初始化函数
 * @arg pdata 初始化的参数结构体地址： struct pwm_led_platform_data *
*/
void pwm_led_init(void *pdata)
{
    __this = (struct pwm_led_platform_data *)pdata;
    if (!__this) {
        return;
    }
    //初始化引脚
    gpio_set_direction(__this->port, 0);
    gpio_set_pull_up(__this->port, 1);
    gpio_set_pull_down(__this->port, 1);
    gpio_set_output_value(__this->port, 1);
    gpio_set_die(__this->port, 1);

    memset((u8 *)JL_PLED, 0, sizeof(JL_PLED_TypeDef));

    P11_P2M_CLK_CON0 &= ~(0b111 << 5);
    P11_P2M_CLK_CON0 |= (0b011 << 5);   //P11_P2M选择lrc_32K时钟源，前提是LRC时钟已经初始化好了
    JL_PLED->CON0 &= ~(0b11 << 2);       //PWM选择P11_P2M做时钟源
    JL_PLED->CON0 &= ~(0b1111 << 4);    //时钟源不分频

    if ((__this->inv_prd & 0xf8) && (!(__this->inv_prd & 0b111))) {
        JL_PLED->CON1 |=  BIT(2);
    } else {
        JL_PLED->CON1 &= ~BIT(2);
    }
    JL_PLED->CON3 |= BIT(4);
    JL_PLED->CON3 |= BIT(6);


    JL_PLED->BRI_PRDH = (__this->pwm_prd >> 8) & 0b11;
    JL_PLED->BRI_PRDL = (__this->pwm_prd >> 0) & 0xff;

    JL_PLED->CON3 &= ~(0b111 << 0); //12位的高3位为0
    JL_PLED->CON3 |= (((__this->t_cnt_prd - 1) >> 8) & 0b111);
    JL_PLED->PRD_DIVL = (__this->t_cnt_prd - 1) & 0xff;

    JL_PLED->CON1 |=  BIT(7);//PWM_DUTY3_EN
    JL_PLED->CON1 |=  BIT(6);//PWM_DUTY2_EN
    JL_PLED->CON1 |=  BIT(5);//PWM_DUTY1_EN
    JL_PLED->CON1 |=  BIT(4);//PWM_DUTY0_EN

    if (__this->mode == 0) {
        JL_PLED->CON2 &= ~(0b1111 << 4);
        JL_PLED->CON2 |= ((__this->inv_prd & 0b111) << 4);
        JL_PLED->BRI_DUTY0H = ((__this->mode_0.h_pwm_duty >> 8) & 0xff);
        JL_PLED->BRI_DUTY0L = ((__this->mode_0.h_pwm_duty >> 0) & 0xff);
        JL_PLED->BRI_DUTY1H = ((__this->mode_0.l_pwm_duty >> 8) & 0xff);
        JL_PLED->BRI_DUTY1L = ((__this->mode_0.l_pwm_duty >> 0) & 0xff);
        JL_PLED->DUTY0 = __this->mode_0.t_cnt0;
        JL_PLED->DUTY1 = __this->mode_0.t_cnt1;
        JL_PLED->DUTY2 = __this->mode_0.t_cnt2;
        JL_PLED->DUTY3 = __this->mode_0.t_cnt3;
    } else {
        JL_PLED->CON2 &= ~(0b1111 << 4);
        JL_PLED->CON2 |= ((__this->inv_prd & 0b111) << 5);
        JL_PLED->CON0 |= BIT(1);    //呼吸变化模式
        JL_PLED->BRI_DUTY0H = ((__this->mode_1.h_pwm_duty_max >> 8) & 0xff);
        JL_PLED->BRI_DUTY0L = ((__this->mode_1.h_pwm_duty_max >> 0) & 0xff);
        JL_PLED->BRI_DUTY1H = ((__this->mode_1.l_pwm_duty_max >> 8) & 0xff);
        JL_PLED->BRI_DUTY1L = ((__this->mode_1.l_pwm_duty_max >> 0) & 0xff);
        JL_PLED->DUTY3 = ((__this->mode_1.pwm_duty_0_cnt >> 8) & 0xff);
        JL_PLED->DUTY2 = ((__this->mode_1.pwm_duty_0_cnt >> 0) & 0xff);
        JL_PLED->DUTY1 = __this->mode_1.h_pwm_duty_max_cnt;
        JL_PLED->DUTY0 = __this->mode_1.l_pwm_duty_max_cnt;
    }
    JL_PLED->CON0 |= BIT(0);
}


/*************************************************  以下是提供的参考示例  *************************************/
// *INDENT-OFF*

#define PWM_LED_BRI_MODE    0

PWM_LED_PLATFORM_DATA_BEGIN(test_pwm_led_pdata)
    .port = IO_PORTC_02,                //引脚
    .inv_prd = 1,                       //h_pwm和l_pwm一个周期交替输出
    .t_cnt_prd = 640,                   //内部独立计数器计数节拍，即最小时间单位 tc = 640 * (1 / 32000) = 20ms, 即每20ms，cnt加1
    .pwm_prd = 320,                     //pwm的周期 = 320 * (1 / 32000) = 10ms
#if PWM_LED_BRI_MODE
    .mode = 1,                          //占空比呼吸模式
    .mode_1.h_pwm_duty_max = 128,       //h_pwm最大占空比，即每过一个pwm周期，占空比变化1，占空比递增时间 = 128 * 10 = 1280ms，占空比递减时间 = 128 * 10 = 1280ms
    .mode_1.l_pwm_duty_max = 240,       //l_pwm最大占空比，即每过一个pwm周期，占空比变化1，占空比递增时间 = 240 * 10 = 2400ms，占空比递减时间 = 240 * 10 = 2400ms
    .mode_1.h_pwm_duty_max_cnt = 25,    //h_pwm占空比增到最大时，维持的时间 = 25 * tc = 25 * 20ms = 500ms
    .mode_1.l_pwm_duty_max_cnt = 50,    //l_pwm占空比增到最大时，维持的时间 = 50 * tc = 50 * 20ms = 1000ms
    .mode_1.pwm_duty_0_cnt = 100,       //pwm呼吸结束后，维持高组态的时间 = 100 * tc = 100 * 20ms = 2000ms
                                        //所以h_pwm的总周期 = 2 * 1280 + 500 + 2000 = 5060ms,  l_pwm的总周期 = 2 * 2400 + 1000 + 2000 = 7400ms
#else
    .mode = 0,                          //占空比固定模式
    .mode_0.h_pwm_duty = 128,           //h_pwm固定占空比：(128 / 320) * 100% = 40%
    .mode_0.l_pwm_duty = 240,           //l_pwm固定占空比: (240 / 320) * 100% = 75%
    .mode_0.t_cnt0 = 64,                //内部计数器cnt在  0  ~  64 期间，控制引脚一直处于高阻状态，不允许PWM波形输出, 时间 = (64 -  0) * tc = 64 * 20 = 1280ms
    .mode_0.t_cnt1 = 80,                //内部计数器cnt在 64  ~  80 期间，控制引脚解除高阻态，允许PWM波形输出, 时间 = (80 - 64) * tc = 16 * 20 = 320ms
    .mode_0.t_cnt2 = 96,                //内部计数器cnt在 80  ~  96 期间，控制引脚一直处于高阻状态，不允许PWM波形输出, 时间 = (96 - 80) * tc = 16 * 20 = 320ms
    .mode_0.t_cnt3 = 112,               //内部计数器cnt在 96  ~ 112 期间，控制引脚解除高阻态，允许PWM波形输出, 时间 = (112 - 96) * tc = 16 * 20 = 320ms
                                        //内部计数器cnt在112  ~ 255 期间，控制引脚一直处于高阻状态，不允许PWM波形输出, 时间 = (256 - 112) * tc = 144 * 20 = 2880ms
                                        //内部计数器数到255后又从0开始数，循环往复，所以该模式的一个计数周期固定是 256 * tc = 256 * 20ms = 5120ms
#endif
PWM_LED_PLATFORM_DATA_END()

//测试代码要放到 lrc 时钟初始化之后
void pwm_led_test(void)
{
    printf("******** pwm led test *************\n");

    pwm_led_init((void *)&test_pwm_led_pdata);

    extern void wdt_clr();
    while (1) {
        wdt_clr();
    }
}


