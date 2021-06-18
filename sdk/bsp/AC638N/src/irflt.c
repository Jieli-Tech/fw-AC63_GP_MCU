#include "asm/irflt.h"

#define IR_INTPUTCHANNEL                7
#define IRFLT_INTPUTCHANNEL_SEL(x)		SFR(JL_IOMAP->CON1, 4, 4, x)

static const u16 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};

JL_TIMER_TypeDef *IR_TIME_REG = NULL;
IR_CODE ir_code;
static u8 cmp_start = 0;
static const struct irflt_platform_data *__this = NULL;

/*
 * @brief   timer中断服务函数
*/
___interrupt
void ir_timer_isr(void)
{
    IR_TIME_REG->CON |= BIT(14);
    u16 bCap1 = IR_TIME_REG->PRD;
    IR_TIME_REG->CNT = 0;
    u8 cap = bCap1 / ir_code.timer_pad;

    ir_code.boverflow = 0;

    if (cmp_start < 3) {
        return;
    }

    /* putchar('0' + (cap / 10)); */
    /* putchar('0' + (cap % 10)); */

    if (cap <= 1) {
        ir_code.wData >>= 1;
        ir_code.bState++;
    } else if (cap == 2) {
        ir_code.wData >>= 1;
        ir_code.wData |= 0x8000;
        ir_code.bState++;
    }

    if (ir_code.bState == 16) {
        ir_code.wUserCode = ir_code.wData;
    }
    if (ir_code.bState == 33) {
        ir_code.bState = 1;
    }
}

/*
 * @brief   红外信号超时检测函数
*/
void ir_timeout(void)
{
    ir_code.boverflow++;
    if (ir_code.boverflow > 56) { //56*2ms ~= 112ms
        ir_code.boverflow = 56;
        ir_code.bState = 0;
    }
    cmp_start ++;
    if (cmp_start > 3) {
        cmp_start = 3;
    }
}
REGISTER_TIMER_TARGET(ir_timeout_det) = {
    .timer_handle = ir_timeout,
};

/*
 * @brief   获取timer输入捕捉红外解码的数值
 * @return  解码后的值
*/
u8 get_irflt_value(void)
{
    u8 tkey = 0xff;
    if (ir_code.bState != 32) {
        return tkey;
    }
    if ((((u8 *)&ir_code.wData)[0] ^ ((u8 *)&ir_code.wData)[1]) == 0xff) {
        tkey = (u8)ir_code.wData;
    } else {
        ir_code.bState = 0;
    }
    return tkey;
}

#define TIMER_UNIT_MS   1
#define MAX_TIME_CNT    0x07ff //分频准确范围，更具实际情况调整
#define MIN_TIME_CNT    0x0030
/*
 * @brief   用于输入捕捉的timer初始化
 * aeg timer_num : 0/1/2/3/4//5 分别对应timer0/1/2/3/4/5
*/
void ir_timer_init(u8 timer_num)
{
    switch (timer_num) {
    case 0:
        IR_TIME_REG = JL_TIMER0;
        request_irq(IRQ_TIME0_IDX, 5, ir_timer_isr, 0);
        break;
    case 1:
        IR_TIME_REG = JL_TIMER1;
        request_irq(IRQ_TIME1_IDX, 5, ir_timer_isr, 0);
        break;
    case 2:
        IR_TIME_REG = JL_TIMER2;
        request_irq(IRQ_TIME2_IDX, 5, ir_timer_isr, 0);
        break;
    case 3:
        IR_TIME_REG = JL_TIMER3;
        request_irq(IRQ_TIME3_IDX, 5, ir_timer_isr, 0);
        break;
    case 4:
        IR_TIME_REG = JL_TIMER4;
        request_irq(IRQ_TIME4_IDX, 5, ir_timer_isr, 0);
        break;
    case 5:
        IR_TIME_REG = JL_TIMER5;
        request_irq(IRQ_TIME5_IDX, 5, ir_timer_isr, 0);
        break;
    default:
        return;
    }
    u32 prd_cnt;
    u8 index;
    u32 timer_clk = 24000000;
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = TIMER_UNIT_MS * (timer_clk / 1000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    cmp_start = 0;
    ir_code.timer_pad = prd_cnt;
    IR_TIME_REG->CON = ((index << 4) | (6 << 10) | BIT(2) | BIT(1) | BIT(0));
}


static u8 ir_io_level = 0;
static u8 ir_io_sus = 0;
/*
 * @brief  IO复用时，IO挂起函数
 * @return 0：挂起成功，  1：挂起是吧
*/
u8 ir_io_suspend(void)
{
    if (!__this) {
        return 1;
    }
    if (ir_io_sus) {
        return 1;
    }
    if (ir_code.boverflow < 7) { //14ms内，红外接收有可能在忙碌
        return 1;
    }
    ir_io_level = gpio_read(__this->ir_io);
    IR_TIME_REG->CON |= BIT(14);
    IR_TIME_REG->CON &= ~(0b11 << 0);
    ir_io_sus = 1;
    return 0;
}

/*
 * @brief  IO复用时，IO恢复函数
 * return  0：恢复成功
*/
u8 ir_io_resume(void)
{
    if (!ir_io_sus) {
        return 0;
    }
    ir_io_sus = 0;
    gpio_set_direction(__this->ir_io, 1);
    gpio_set_die(__this->ir_io, 1);
    gpio_set_pull_up(__this->ir_io, 1);
    gpio_set_pull_down(__this->ir_io, 0);
    delay(10);
    if ((ir_io_level) && (ir_io_level != (gpio_read(__this->ir_io)))) {
        ir_code.boverflow = 0;
    }
    cmp_start = 0;
    IR_TIME_REG->CNT = 0;
    IR_TIME_REG->CON |= BIT(14);
    IR_TIME_REG->CON |= (0b11 << 0);
    return 0;
}

/*
 * @brief  红外初始化函数
 * @arg pdata 初始化的参数结构体地址 ： struct irflt_platform_data *
*/
void irflt_init(void *pdata)
{
    __this = (const struct irflt_platform_data *)pdata;
    if (!__this) {
        return;
    }

    IRFLT_INTPUTCHANNEL_SEL(IR_INTPUTCHANNEL);
    gpio_set_fun_input_port(__this->ir_io, PFI_GP_ICH0 + IR_INTPUTCHANNEL * 4);
    gpio_set_direction(__this->ir_io, 1);
    gpio_set_die(__this->ir_io, 1);
    gpio_set_pull_up(__this->ir_io, 1);
    gpio_set_pull_down(__this->ir_io, 0);

    JL_IR->RFLT_CON = 0;
    JL_IR->RFLT_CON |= BIT(7) | BIT(4);		//512 div
    JL_IR->RFLT_CON |= BIT(3) | BIT(2);		//PLL_48m（兼容省晶振）
    JL_IR->RFLT_CON |= BIT(0);		        //irflt enable

    ir_timer_init(__this->timer_num);
}

/*************************** 以下是SDK参考示例 ****************************/
// *INDENT-OFF*
IRFLT_PLATFORM_DATA_BEGIN(test_irflt_pdata)
    .ir_io = IO_PORTC_02,   //红外信号捕获引脚
    .timer_num = 5,         //选用timer5
IRFLT_PLATFORM_DATA_END()

void irflt_test(void)
{
    printf("*****************  irflt test  ***************\n");
    irflt_init((void *)&test_irflt_pdata);

    u8 ir_val = 0;
    extern void wdt_clr();
    while (1) {
        wdt_clr();
        ir_val = get_irflt_value();
        if (ir_val != 0xff) {
            printf("ir_val = %d\n", ir_val);
        }
        delay(100000);
    }
}


