
//===============================================================================//
//
//      input IO define
//
//===============================================================================//
#define PA0_IN      1
#define PA1_IN      2
#define PA2_IN      3
#define PA3_IN      4
#define PA4_IN      5
#define PA5_IN      6
#define PA6_IN      7
#define PA7_IN      8
#define PA8_IN      9
#define PB0_IN      10
#define PB1_IN      11
#define PB2_IN      12
#define PB3_IN      13
#define PB4_IN      14
#define PB5_IN      15
#define PB6_IN      16
#define PB7_IN      17
#define PB8_IN      18
#define PC0_IN      19
#define PC1_IN      20
#define PC2_IN      21
#define PC3_IN      22
#define PC4_IN      23
#define PC5_IN      24
#define PD0_IN      25
#define PD1_IN      26
#define PD2_IN      27
#define PD3_IN      28
#define PD4_IN      29
#define PD5_IN      30
#define PD6_IN      31
#define PD7_IN      32
#define USBDP_IN    33
#define USBDM_IN    34
#define PP0_IN      35

//  Only for p33 port mux in (without crossbar)
//************************************************
#define P00_IN      36
#define CHGFL_IN    37
#define VBGOK_IN    38
#define VBTCH_IN    39
#define LINDT_IN    40

//===============================================================================//
//
//      function input select sfr
//
//===============================================================================//
typedef struct {
    __RW __u8 FI_GP_ICH0;
    __RW __u8 FI_GP_ICH1;
    __RW __u8 FI_GP_ICH2;
    __RW __u8 FI_GP_ICH3;
    __RW __u8 FI_GP_ICH4;
    __RW __u8 FI_GP_ICH5;
    __RW __u8 FI_GP_ICH6;
    __RW __u8 FI_GP_ICH7;
    __RW __u8 FI_TMR0_CIN;
    __RW __u8 FI_TMR0_CAP;
    __RW __u8 FI_TMR1_CIN;
    __RW __u8 FI_TMR1_CAP;
    __RW __u8 FI_TMR2_CIN;
    __RW __u8 FI_TMR2_CAP;
    __RW __u8 FI_TMR3_CIN;
    __RW __u8 FI_TMR3_CAP;
    __RW __u8 FI_TMR4_CIN;
    __RW __u8 FI_TMR4_CAP;
    __RW __u8 FI_TMR5_CIN;
    __RW __u8 FI_TMR5_CAP;
    __RW __u8 FI_SPI0_CLK;
    __RW __u8 FI_SPI0_DA0;
    __RW __u8 FI_SPI0_DA1;
    __RW __u8 FI_SPI0_DA2;
    __RW __u8 FI_SPI0_DA3;
    __RW __u8 FI_SPI1_CLK;
    __RW __u8 FI_SPI1_DA0;
    __RW __u8 FI_SPI1_DA1;
    __RW __u8 FI_SPI1_DA2;
    __RW __u8 FI_SPI1_DA3;
    __RW __u8 FI_SPI2_CLK;
    __RW __u8 FI_SPI2_DA0;
    __RW __u8 FI_SPI2_DA1;
    __RW __u8 FI_SPI2_DA2;
    __RW __u8 FI_SPI2_DA3;
    __RW __u8 FI_SD0_CMD;
    __RW __u8 FI_SD0_DA0;
    __RW __u8 FI_SD0_DA1;
    __RW __u8 FI_SD0_DA2;
    __RW __u8 FI_SD0_DA3;
    __RW __u8 FI_IIC_SCL;
    __RW __u8 FI_IIC_SDA;
    __RW __u8 FI_UART0_RX;
    __RW __u8 FI_UART1_RX;
    __RW __u8 FI_UART1_CTS;
    __RW __u8 FI_UART2_RX;
    __RW __u8 FI_RDEC0_DAT0;
    __RW __u8 FI_RDEC0_DAT1;
    __RW __u8 FI_ALNK0_MCLK;
    __RW __u8 FI_ALNK0_LRCK;
    __RW __u8 FI_ALNK0_SCLK;
    __RW __u8 FI_ALNK0_DAT0;
    __RW __u8 FI_ALNK0_DAT1;
    __RW __u8 FI_ALNK0_DAT2;
    __RW __u8 FI_ALNK0_DAT3;
    __RW __u8 FI_PLNK_DAT0;
    __RW __u8 FI_PLNK_DAT1;
    __RW __u8 FI_CHAIN_IN0;
    __RW __u8 FI_CHAIN_IN1;
    __RW __u8 FI_CHAIN_IN2;
    __RW __u8 FI_CHAIN_IN3;
    __RW __u8 FI_CHAIN_RST;
    __RW __u8 FI_TOTAL;
} JL_IMAP_TypeDef;

#define JL_IMAP_BASE      (ls_base + map_adr(0x57, 0x00))
#define JL_IMAP           ((JL_IMAP_TypeDef   *)JL_IMAP_BASE)

