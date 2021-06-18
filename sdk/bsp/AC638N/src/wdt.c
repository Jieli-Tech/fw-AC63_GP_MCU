#include "typedef.h"
#include "asm/includes.h"

#ifdef CONFIG_RELEASE_ENABLE
#define     INTERRUPT_MODE          0
#else
#define     INTERRUPT_MODE          1
#endif

static const u32 wdt_time[] = {
    1,    2,     4,     8,
    16,   32,    64,   128,
    256,  512,  1024,  2048,
    4096, 8192, 16384, 32768,
};

void wdt_close(void)
{
    P11_WDT_CON0 &= ~BIT(4);
}


void wdt_enable(void)
{
    P11_WDT_CON0 |= BIT(0);
}

void wdt_disable(void)
{
    P11_WDT_CON0 &= ~BIT(4);
}

void wdt_clear(void)
{
    P11_WDT_CON0 |= BIT(6);
}

void clr_wdt(void)
{
    wdt_clear();
}

void wdt_set_irq(void *handler)
{
    P11_WDT_CON0 |= BIT(5);
    P11_WDT_CON0 |= BIT(6);
    wdt_enable();
}

void wdt_clr_pending(void)
{
    wdt_clear();
}

u32 __hw_wakeup_source(void);

void wdt_isr(void)
{
    u32 tmp;
    u32 rets, reti;

    __asm__ volatile("%0 = rets" : "=r"(rets));
    __asm__ volatile("%0 = reti" : "=r"(reti));

    printf("\r\n\r\n---------------------------watchdog---------------------\r\n\r\n");
    printf("RETS = %08x \r\n", rets);
    printf("RETI = %08x \r\n", reti);

    __asm__ volatile("%0 = sp" : "=r"(tmp));
    printf("SP = %08x \r\n", tmp);
    __asm__ volatile("%0 = usp" : "=r"(tmp));
    printf("USP = %08x \r\n", tmp);
    __asm__ volatile("%0 = ssp" : "=r"(tmp));
    printf("SSP = %08x \r\n", tmp);
    while (1);
}


void wdt_init(u8 time)
{
#if INTERRUPT_MODE
    P11_WDT_CON0 = BIT(4) | BIT(5) | (time & 0xf);
    P11_WDT_CON1 = BIT(0);
#else
    P11_WDT_CON0 = BIT(4) | (time & 0xf);
    P11_WDT_CON1 = 0;
#endif
}

u32 wdt_get_time(void)
{
    if (P11_WDT_CON0 & BIT(4)) {
        wdt_clear();
        return wdt_time[P11_WDT_CON0 & 0x0f];
    }
    return 0;
}

