#ifndef HWI_H
#define HWI_H


#define IRQ_EMUEXCPT_IDX   0		//0
#define IRQ_EXCEPTION_IDX  1		//0
#define IRQ_SYSCALL_IDX    2		//0
#define IRQ_TICK_TMR_IDX   3		//0

#define IRQ_TIME0_IDX      4   	//0
#define IRQ_TIME1_IDX      5   	//0
#define IRQ_TIME2_IDX      6   	//0
#define IRQ_TIME3_IDX      7   	//0
#define IRQ_USB_SOF_IDX    8   	//1
#define IRQ_USB_CTRL_IDX   9  	//1
#define IRQ_P2M_IDX        10  	//0
#define IRQ_PORT_IDX       13		//0
#define IRQ_SPI0_IDX       14		//0
#define IRQ_SPI1_IDX       15		//0
#define IRQ_SD0_IDX        16		//0
#define IRQ_SD1_IDX        17		//0
#define IRQ_UART0_IDX      18		//0
#define IRQ_UART1_IDX      19		//0
#define IRQ_UART2_IDX      20		//0
#define IRQ_PAP_IDX        21		//0
#define IRQ_IIC_IDX        22		//0
#define IRQ_SARADC_IDX     23		//0
#define IRQ_PDM_LINK_IDX   24		//1
#define IRQ_RDEC0_IDX      25		//1
#define IRQ_LRCT_IDX       26       //1
#define IRQ_FFT_IDX        32 		//1
#define IRQ_LP_TIMER0_IDX  34
#define IRQ_LP_TIMER1_IDX  35
#define IRQ_AES_IDX        40
#define IRQ_MCTMRX_IDX 	   41
#define IRQ_CHX_PWM_IDX    42
#define IRQ_SPI2_IDX       56
#define IRQ_GPC_IDX		   46		//1
#define IRQ_FMTX_IDX	   47		//1
#define IRQ_DCP_IDX	       48		//1
#define IRQ_RDEC1_IDX      49		//1
#define IRQ_RDEC2_IDX      50		//1
#define IRQ_RDEC3_IDX      51		//1
#define IRQ_PWM_LED_IDX    52		//1
#define IRQ_CTM_IDX        53		//1
#define IRQ_TIME4_IDX      54   	//0
#define IRQ_TIME5_IDX      55   	//0
#define IRQ_SOFT0_IDX      60
#define IRQ_SOFT1_IDX      61
#define IRQ_SOFT2_IDX      62
#define IRQ_SOFT3_IDX      63

#define IRQ_MEM_ADDR        0x031F00
#define MAX_IRQ_ENTRY_NUM   64


static inline int core_num(void)
{
    return 0;
}

void interrupt_init();

void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);

void unrequest_irq(u8 index);

void bit_clr_ie(unsigned char index);

u32 irq_check_ipnd(u32 index);


//---------------------------------------------//
// low power waiting
//---------------------------------------------//
__attribute__((always_inline))
static void lp_waiting(int *ptr, int pnd, int cpd, char inum)
{
    q32DSP(core_num())->IWKUP_NUM = inum;
    while (!(*ptr & pnd)) {
        asm volatile("idle");
    }
    *ptr |= cpd;
}


#endif

