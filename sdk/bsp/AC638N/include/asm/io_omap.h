//===============================================================================//
//
//      output function define
//
//===============================================================================//
#define FO_GP_OCH0        0
#define FO_GP_OCH1        1
#define FO_GP_OCH2        2
#define FO_GP_OCH3        3
#define FO_GP_OCH4        4
#define FO_GP_OCH5        5
#define FO_GP_OCH6        6
#define FO_GP_OCH7        7
#define FO_GP_OCH8        8
#define FO_GP_OCH9        9
#define FO_GP_OCH10       10
#define FO_GP_OCH11       11
#define FO_TMR0_PWM       12
#define FO_TMR1_PWM       13
#define FO_TMR2_PWM       14
#define FO_TMR3_PWM       15
#define FO_TMR4_PWM       16
#define FO_TMR5_PWM       17
#define FO_SPI0_CLK       18
#define FO_SPI0_DA0       19
#define FO_SPI0_DA1       20
#define FO_SPI0_DA2       21
#define FO_SPI0_DA3       22
#define FO_SPI1_CLK       23
#define FO_SPI1_DA0       24
#define FO_SPI1_DA1       25
#define FO_SPI1_DA2       26
#define FO_SPI1_DA3       27
#define FO_SPI2_CLK       28
#define FO_SPI2_DA0       29
#define FO_SPI2_DA1       30
#define FO_SPI2_DA2       31
#define FO_SPI2_DA3       32
#define FO_SD0_CLK        33
#define FO_SD0_CMD        34
#define FO_SD0_DA0        35
#define FO_SD0_DA1        36
#define FO_SD0_DA2        37
#define FO_SD0_DA3        38
#define FO_IIC_SCL        39
#define FO_IIC_SDA        40
#define FO_UART0_TX       41
#define FO_UART1_TX       42
#define FO_UART1_RTS      43
#define FO_UART2_TX       44
#define FO_ALNK0_MCLK     45
#define FO_ALNK0_LRCK     46
#define FO_ALNK0_SCLK     47
#define FO_ALNK0_DAT0     48
#define FO_ALNK0_DAT1     49
#define FO_ALNK0_DAT2     50
#define FO_ALNK0_DAT3     51
#define FO_PLNK_SCLK      52
#define FO_CHAIN_OUT0     53
#define FO_CHAIN_OUT1     54
#define FO_CHAIN_OUT2     55
#define FO_CHAIN_OUT3     56

//===============================================================================//
//
//      IO output select sfr
//
//===============================================================================//
typedef struct {
    __RW __u8 PA0_OUT;
    __RW __u8 PA1_OUT;
    __RW __u8 PA2_OUT;
    __RW __u8 PA3_OUT;
    __RW __u8 PA4_OUT;
    __RW __u8 PA5_OUT;
    __RW __u8 PA6_OUT;
    __RW __u8 PA7_OUT;
    __RW __u8 PA8_OUT;
    __RW __u8 PB0_OUT;
    __RW __u8 PB1_OUT;
    __RW __u8 PB2_OUT;
    __RW __u8 PB3_OUT;
    __RW __u8 PB4_OUT;
    __RW __u8 PB5_OUT;
    __RW __u8 PB6_OUT;
    __RW __u8 PB7_OUT;
    __RW __u8 PB8_OUT;
    __RW __u8 PC0_OUT;
    __RW __u8 PC1_OUT;
    __RW __u8 PC2_OUT;
    __RW __u8 PC3_OUT;
    __RW __u8 PC4_OUT;
    __RW __u8 PC5_OUT;
    __RW __u8 PD0_OUT;
    __RW __u8 PD1_OUT;
    __RW __u8 PD2_OUT;
    __RW __u8 PD3_OUT;
    __RW __u8 PD4_OUT;
    __RW __u8 PD5_OUT;
    __RW __u8 PD6_OUT;
    __RW __u8 PD7_OUT;
    __RW __u8 USBDP_OUT;
    __RW __u8 USBDM_OUT;
    __RW __u8 PP0_OUT;
} JL_OMAP_TypeDef;

#define JL_OMAP_BASE      (ls_base + map_adr(0x56, 0x00))
#define JL_OMAP           ((JL_OMAP_TypeDef   *)JL_OMAP_BASE)

