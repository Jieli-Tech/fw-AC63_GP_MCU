#ifndef __CLOCK_HW_H__
#define __CLOCK_HW_H__

#include "typedef.h"

// #define CLOCK_HAL_DEBUG

#ifdef CLOCK_HAL_DEBUG

#define RC_EN(x)
#define TEST_SEL(x)
#define OSC_CLOCK_IN(x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC_H,
    OSC_CLOCK_IN_RTOSC_L,
    OSC_CLOCK_IN_PAT,
};

#define MAIN_CLOCK_SEL(x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_RC = 0,

    MAIN_CLOCK_IN_BTOSC = 4,
    MAIN_CLOCK_IN_RTOSC_H,
    MAIN_CLOCK_IN_RTOSC_L,
    MAIN_CLOCK_IN_PLL,

    MAIN_CLOCK_IN_PAT, //for tes
};

#define SFR_MODE(x)
enum {
    SFR_CLOCK_IDLE = 0,
    SFR_CLOCK_ALWAYS_ON,
};
#define PB0_CLOCK_OUT(x)
#define PA2_CLOCK_OUT(x)


#define USB_CLOCK_IN(x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_DISABLE,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE_PAD,
};
#define DAC_CLOCK_IN(x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_OSC,
    DAC_CLOCK_IN_LSB,
    DAC_CLOCK_IN_DISABLE_PAD,
};
#define APC_CLOCK_IN(x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLL64M = 0,
    APC_CLOCK_IN_PLLAPC,
    APC_CLOCK_IN_LSB,
    APC_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_PLL48M = 0,
    UART_CLOCK_IN_OSC,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_PLL64M = 0,
    BT_CLOCK_IN_DISABLE,
    BT_CLOCK_IN_LSB,
    BT_CLOCK_IN_DISABLE_PAD,
};


#define SFC_CLOCK_DELAY(x)


#define PLL_SYS_SEL(x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL137M,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)
//for MACRO - PLL_SYS_DIV
enum {
    PLL_SYS_DIV1 = 0,
    PLL_SYS_DIV3,
    PLL_SYS_DIV5,
    PLL_SYS_DIV7,

    PLL_SYS_DIV1X2 = 4,
    PLL_SYS_DIV3X2,
    PLL_SYS_DIV5X2,
    PLL_SYS_DIV7X2,

    PLL_SYS_DIV1X4 = 8,
    PLL_SYS_DIV3X4,
    PLL_SYS_DIV5X4,
    PLL_SYS_DIV7X4,

    PLL_SYS_DIV1X8 = 12,
    PLL_SYS_DIV3X8,
    PLL_SYS_DIV5X8,
    PLL_SYS_DIV7X8,
};

#define PLL_APC_SEL(x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL137M,
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)
//for MACRO - PLL_APC_DIV
enum {
    PLL_APC_DIV1 = 0,
    PLL_APC_DIV3,
    PLL_APC_DIV5,
    PLL_APC_DIV7,

    PLL_APC_DIV1X2 = 4,
    PLL_APC_DIV3X2,
    PLL_APC_DIV5X2,
    PLL_APC_DIV7X2,

    PLL_APC_DIV1X4 = 8,
    PLL_APC_DIV3X4,
    PLL_APC_DIV5X4,
    PLL_APC_DIV7X4,

    PLL_APC_DIV1X8 = 12,
    PLL_APC_DIV3X8,
    PLL_APC_DIV5X8,
    PLL_APC_DIV7X8,
};

#define PLL_ALNK_SEL(x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_EN(x)
#define PLL_REST(x)
#define PLL_DIVn(x)
#define PLL_DIVn_EN(x)
#define PLL_REF_SEL(x)
//for MACRO - PLL_RSEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//bt
    PLL_REF_SEL_RTOSC,		//rt
    PLL_REF_SEL_PAT = 3,
};

#define PLL_TEST(x)
#define PLL_DSMS(x)
#define PLL_DSM_TSEL(x)
#define PLL_DSM_RSEL(x)
#define PLL_DSM_MSEL(x)

#define PLL_DIVSEL(x)

//for MACRO - PLL_RSEL
enum {
    PLL_DIVIDER_INTE = 0,
    PLL_DIVIDER_FRAC,
};

#define PLL_PFD(x)
#define PLL_ICP(x)
#define PLL_LPFR2(x)
#define PLL_LPFR3(x)


#define PLL_RSEL(x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_BTOSC_DIFF = 0,
    PLL_RSEL_RTOSC_DIFF,
    PLL_RSEL_PLL_REF_SEL,
};

#define PLL_DIVm(x)
#define PLL_LD012A(x)
#define PLL_LDO12D(x)
#define PLL_IVCO(x)
#define PLL_LDO_BYPASS(x)
#define PLL_TSSEL(x)
#define PLL_TSOE(x)

#define PLL_CLK480M_OE(x)
#define PLL_CLK320M_OE(x)
#define PLL_CLK192M_OE(x)
#define PLL_CLK137M_OE(x)
#define PLL_CLK107M_OE(x)

#define PLL_FRAC(x)
#define PLL_DMAX(x)
#define PLL_DMIN(x)
#define PLL_DSTP(x)

#define HSB_CLK_DIV(x)
#define LSB_CLK_DIV(x)
#define OTP_CLK_DIV(x)


#else

#define RC_EN(x)                SFR(JL_CLOCK->CLK_CON0,  0,  1,  x)
#define RCH_EN(x)				SFR(JL_CLOCK->CLK_CON0,  0,  1,  x)
//for MACRO - RCH_EN
enum {
    RCH_EN_250K = 0,
    RCH_EN_16M,
};

#define TEST_SEL(x)				SFR(JL_CLOCK->CLK_CON0,  2,  2,  x)
//for MACRO - TS_SEL
enum {
    TS_SEL_IN_MAIN = 0,
    TS_SEL_IN_PAT,
};

#define OSC_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON0,  1,  3,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_BT_OSC_X2,
    OSC_CLOCK_IN_STD_24M,
    OSC_CLOCK_IN_RTC_OSC,
    OSC_CLOCK_IN_LRC,
    OSC_CLOCK_IN_PAT,
};


#define MAIN_CLOCK_SEL(x) 		SFR(JL_CLOCK->CLK_CON1,  8,  3,  x); \
									asm("csync")

//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_RC_250K = 0,
    MAIN_CLOCK_IN_PAT,
    MAIN_CLOCK_IN_RTC_OSC,
    MAIN_CLOCK_IN_RC,
    MAIN_CLOCK_IN_BTOSC,
    MAIN_CLOCK_IN_BTOSC_X2,
    MAIN_CLOCK_IN_PLL,
};


#define SFR_MODE(x)             SFR(JL_CLOCK->CLK_CON1,  11,  1,  x)
enum {
    SFR_CLOCK_IDLE = 0,
    SFR_CLOCK_ALWAYS_ON,
};


#define PLL_48M_SEL(x)          SFR(JL_CLOCK->CLK_CON0, 9, 1,  x)
enum {
    PLL_48M_SEL_DIV2 = 0,
    PLL_48M_SEL_DIV1,
};

#define PLL_48M_SEL_GET()       ((JL_CLOCK->CLK_CON0 & BIT(9)) >> 9)

#define PLL_96M_SEL(x)          SFR(JL_CLOCK->CLK_CON0, 4, 5,  x)
enum {
    PLL_96M_SEL_NULL = 0,

    PLL_96M_SEL_9DIV2, 	//f5
    PLL_96M_SEL_7DIV2, 	//f4
    PLL_96M_SEL_5DIV2,	//f3
    PLL_96M_SEL_3DIV2,	//f2
    PLL_96M_SEL_1DIV1, 	//f1

    PLL_96M_SEL_9DIV4 = 0x11, 	//f5 / 2
    PLL_96M_SEL_7DIV4,			//f4 / 2
    PLL_96M_SEL_5DIV4,			//f3 / 2
    PLL_96M_SEL_3DIV4,			//f2 / 2
    PLL_96M_SEL_1DIV2,			//f1 / 2

    PLL_96M_SEL_END
};

#define PLL_96M_SEL_GET()       ((JL_CLOCK->CLK_CON0 >> 4) & 0x1F)

#define PLL_STD_SEL(x)          SFR(JL_CLOCK->CLK_CON0, 10, 1,  x)

#define USB_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON2, 8, 2,  x)

#define PLL_ALNK0_SEL(x)        SFR(JL_CLOCK->CLK_CON2, 0, 2,  x)
#define PLL_ALNK0_DIV(x)        SFR(JL_CLOCK->CLK_CON2, 2, 2,  x)

//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_STD48M = 0,
    USB_CLOCK_IN_OSC,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE,
};
#define AUDIO_CLOCK_IN(x)       SFR(JL_CLOCK->CLK_CON1,  2,  2,  x)
//for MACRO - AUDIO_CLOCK_IN
enum {
    AUDIO_CLOCK_IN_PLL48M = 0,
    AUDIO_CLOCK_IN_OSC,
    AUDIO_CLOCK_IN_LSB,
    AUDIO_CLOCK_IN_DISABLE,
};
#define GPCNT_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON2,  29,  3,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    GPCNT_CLOCK_IN_NULL = 0,
    GPCNT_CLOCK_IN_SRC,
    GPCNT_CLOCK_IN_SFC,
    GPCNT_CLOCK_IN_HSB,
    GPCNT_CLOCK_IN_XOSC_FSCK,
    GPCNT_CLOCK_IN_AUDIO,
    GPCNT_CLOCK_IN_WL,
    GPCNT_CLOCK_IN_USB,
};

#define UART_CLOCK_IN(x)        SFR(JL_CLOCK->CLK_CON2,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_STD24M = 0,
    UART_CLOCK_IN_OSC,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)          SFR(JL_CLOCK->CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_STD48M = 0,
    BT_CLOCK_IN_OSC,
    BT_CLOCK_IN_HSB,
    BT_CLOCK_IN_DISABLE,
};

#define SFC_SCKE(x)             SFR(JL_CLOCK->CLK_CON1,  17,  1,  x)

#define WL2ADC_CLOCK_IN(x)      SFR(JL_CLOCK->CLK_CON1,  18,  2,  x)
//for MACRO - WL2ADC_CLOCK_IN
enum {
    WL2ADC_CLOCK_IN_PLL96M = 0,
    WL2ADC_CLOCK_IN_STD48M,
    WL2ADC_CLOCK_IN_HSB,
    WL2ADC_CLOCK_IN_DISABLE,
};

#define WL2DAC_CLOCK_IN(x)      SFR(JL_CLOCK->CLK_CON1,  21,  2,  x)
//for MACRO - WL2DAC_CLOCK_IN
enum {
    WL2DAC_CLOCK_IN_PLL96M = 0,
    WL2DAC_CLOCK_IN_HSB,
    WL2DAC_CLOCK_IN_LSB,
    WL2DAC_CLOCK_IN_DISABLE,
};


#define SFC_CLOCK_DELAY(x)      SFR(JL_CLOCK->CLK_CON1,  28,  2,  x)


#define PLL_CLK_EN(x)         	SFR(JL_PLL->PLL_CON3, 9, 1,  x)

#define PLL_SYS_SEL(x)          SFR(JL_CLOCK->CLK_CON1,  0,  3,  x)
#define PLL_SYS_SEL_GET()       ((JL_CLOCK->CLK_CON1 & 0x7))

//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_NULL = 0,
    PLL_SYS_SEL_9DIV2, //f5
    PLL_SYS_SEL_7DIV2, //f4
    PLL_SYS_SEL_5DIV2, //f3
    PLL_SYS_SEL_3DIV2, //f2
    PLL_SYS_SEL_1DIV1, //f1

    PLL_SYS_SEL_END
};

#define PLL_SYS_DIV(x)          SFR(JL_CLOCK->CLK_CON1,  4,  4,  x)
//for MACRO - PLL_SYS_DIV
enum {
    PLL_SYS_DIV1 = 0,
    PLL_SYS_DIV3,
    PLL_SYS_DIV5,
    PLL_SYS_DIV7,

    PLL_SYS_DIV1X2 = 4,
    PLL_SYS_DIV3X2,
    PLL_SYS_DIV5X2,
    PLL_SYS_DIV7X2,

    PLL_SYS_DIV1X4 = 8,
    PLL_SYS_DIV3X4,
    PLL_SYS_DIV5X4,
    PLL_SYS_DIV7X4,

    PLL_SYS_DIV1X8 = 12,
    PLL_SYS_DIV3X8,
    PLL_SYS_DIV5X8,
    PLL_SYS_DIV7X8,
};

#define PLL_ALNK_EN(x)         SFR(JL_CLOCK->CLK_CON2,  6,  1,  x)
#define PLL_ALNK_SEL(x)        SFR(JL_CLOCK->CLK_CON2,  7,  1,  x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_FM_SEL(x)	        SFR(JL_CLOCK->CLK_CON2,  12,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL137M,
    PLL_APC_SEL_PLL107M,
    PLL_APC_SEL_DISABLE,
};
#define PLL_FM_DIV(x)	        SFR(JL_CLOCK->CLK_CON2,  14,  4,  x)
//for MACRO - PLL_APC_DIV
enum {
    PLL_FM_DIV1 = 0,
    PLL_FM_DIV3,
    PLL_FM_DIV5,
    PLL_FM_DIV7,

    PLL_FM_DIV1X2 = 4,
    PLL_FM_DIV3X2,
    PLL_FM_DIV5X2,
    PLL_FM_DIV7X2,

    PLL_FM_DIV1X4 = 8,
    PLL_FM_DIV3X4,
    PLL_FM_DIV5X4,
    PLL_FM_DIV7X4,

    PLL_FM_DIV1X8 = 12,
    PLL_FM_DIV3X8,
    PLL_FM_DIV5X8,
    PLL_FM_DIV7X8,
};

#define DPLL_UDEN(x)            SFR(JL_CLOCK->CLK_CON2,  30,  1,  x)

// #define DSP_RESET(x)            SFR(JL_CLOCK->CLK_CON3,  0,  1,  x)

// #define DSP_POWER_RESET(x)      SFR(JL_CLOCK->CLK_CON3,  1,  1,  x)



#define PLL_EN(x)         		SFR(JL_PLL->PLL_CON0,  0,  1,  x)
#define PLL_REST(x)             SFR(JL_PLL->PLL_CON0,  1,  1,  x)

// #define PLL_TEST(x)         	SFR(JL_PLL->PLL_CON0,  10, 1,  x)

// #define PLL_DIVS(x)             SFR(JL_PLL->PLL_CON0,  20, 2,  x)
#define PLL_PFD(x)              SFR(JL_PLL->PLL_CON0,  3, 2,  x)
#define PLL_ICP(x)              SFR(JL_PLL->PLL_CON0,  5, 3,  x)
#define PLL_LPFR2(x)            SFR(JL_PLL->PLL_CON0,  8, 3,  x)
#define PLL_IVCO(x)             SFR(JL_PLL->PLL_CON0, 11, 3,  x)
#define PLL_LDO12A(x)           SFR(JL_PLL->PLL_CON0, 14, 3,  x)
#define PLL_LDO12D(x)           SFR(JL_PLL->PLL_CON0, 17, 3,  x)
#define PLL_LDO_BYPASS(x)       SFR(JL_PLL->PLL_CON0, 20, 1,  x)


#define PLL_DIVn(x)             SFR(JL_PLL->PLL_CON1,  0,  7,  x)

#define PLL_REF_SEL1(x)        	SFR(JL_PLL->PLL_CON1,  7, 2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_RCLK = 0, 	//
    PLL_RSEL_RCH,
    PLL_RSEL_DPLL_CLK,
    PLL_RSEL_PAT_CLK,
};

#define PLL_REF_SEL(x)        	SFR(JL_PLL->PLL_CON1,  9,  1,  x)
//for MACRO - PLL_REF_SEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//btosc
    PLL_REF_SEL_RCLK,
};

#define PLL_DIVn_EN(x)          SFR(JL_PLL->PLL_CON1, 10, 2,  x)
//for MACRO - PLL_DIVn_EN
enum {
    PLL_DIVn_EN_X2 = 0,
    PLL_DIVn_DIS_DIV1,
    PLL_DIVn_EN2_33,
};



// #define PLL_DAC_OE(x)        SFR(JL_PLL->PLL_CON1, 30, 1,  x)

#define PLL_FBDS(x)             SFR(JL_PLL->PLL_CON2, 0,  12,  x)

#define PLL_CLK_1DIV1_OE(x)     SFR(JL_PLL->PLL_CON3, 0, 1,  x)
#define PLL_CLK_3DIV2_OE(x)     SFR(JL_PLL->PLL_CON3, 1, 1,  x)
#define PLL_CLK_5DIV2_OE(x)     SFR(JL_PLL->PLL_CON3, 2, 1,  x)
#define PLL_CLK_7DIV2_OE(x)     SFR(JL_PLL->PLL_CON3, 3, 1,  x)
#define PLL_CLK_9DIV2_OE(x)     SFR(JL_PLL->PLL_CON3, 4, 1,  x)


#define PLL_DSMS(x)         	SFR(JL_PLL->PLL_CON4,  0, 1,  x)
#define PLL_TSEL(x)         	SFR(JL_PLL->PLL_CON4,  1, 4,  x)
#define PLL_RSEL(x)         	SFR(JL_PLL->PLL_CON4,  5, 2,  x)
#define PLL_MSEL(x)         	SFR(JL_PLL->PLL_CON4,  7, 2,  x)



#define HSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  8,  3,  x)
#define SFC_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  12, 3,  x)

/********************************************************************************/
#define GPCNT_EN(x)             SFR(JL_GPCNT->CON,  0,  1,  x)
#define GPCNT_CSS(x)            SFR(JL_GPCNT->CON,  1,  3,  x)
//for MACRO - GPCNT_CSS
enum {
    GPCNT_CSS_LSB = 0,
    GPCNT_CSS_OSC,
    GPCNT_CSS_INPUT_CH2,    //iomap con1[8:11]
    GPCNT_CSS_INPUT_CH3,    //iomap con1[12:15]
    GPCNT_CSS_CLOCK_IN,
    GPCNT_CSS_RING,
    GPCNT_CSS_PLL,
    GPCNT_CSS_INTPUT_CH1,   //iomap con1[4:7]
};

#define GPCNT_CLR_PEND(x)       SFR(JL_GPCNT->CON,  6,  1,  x)
#define GPCNT_GTS(x)            SFR(JL_GPCNT->CON,  8,  4,  x)

#define GPCNT_GSS(x)            SFR(JL_GPCNT->CON,  12, 3,  x)
//for MACRO - GPCNT_CSS
enum {
    GPCNT_GSS_LSB = 0,
    GPCNT_GSS_OSC,
    GPCNT_GSS_INPUT_CH2,    //iomap con1[8:11]
    GPCNT_GSS_INPUT_CH3,    //iomap con1[12:15]
    GPCNT_GSS_CLOCK_IN,
    GPCNT_GSS_RING,
    GPCNT_GSS_PLL480M,
    GPCNT_GSS_INTPUT_CH1,   //iomap con1[4:7]
};



#define lrc_con1_init                                 \
    /*                               */     ((0 << 7) |\
    /*RC32K_CAP_S2_33v               */     (0 << 6) |\
    /*RC32K_CAP_S1_33v               */     (1 << 5) |\
    /*RC32K_CAP_S0_33v               */     (1 << 4) |\
    /*                               */     (0 << 3) |\
    /*                               */     (0 << 2) |\
    /*RC32K_RNPS_S1_33v              */     (0 << 1) |\
    /*RC32K_RNPS_S0_33v              */     (1 << 0))

#define lrc_con0_init                                 \
    /*                               */     ((0 << 7) |\
    /*                               */     (0 << 6) |\
    /*RC32K_RPPS_S1_33v              */     (0 << 5) |\
    /*RC32K_RPPS_S0_33v              */     (1 << 4) |\
    /*                               */     (0 << 3) |\
    /*                               */     (0 << 2) |\
    /*RC32K_RN_TRIM_33v              */     (0 << 1) |\
    /*RC32K_EN_33v                   */     (1 << 0))




#define lrc_pll_con4                               \
    /*RST                       1 bit*/     ((     0 << 9  ) |\
    /*SYSPLL_MSEL(1-0)          2 bit*/     (   0b00 << 7  ) |\
    /*SYSPLL_RSEL(1-0)          2 bit*/     (   0b00 << 5  ) |\
    /*SYSPLL_TSEL(3-0)          4 bit*/     ( 0b0000 << 1  ) |\
    /*SYSPLL_DSMS               1 bit*/     (      0 << 0 ))

#define lrc_pll_con3                               \
    /*SYSPLL_CKE                1 bit*/     ((     1 << 9  ) |\
    /*DIVS(2-0)_BTADC           3 bit*/     (  0b100 << 6  ) |\
    /*CKOE_BTADC                1 bit*/     (      0 << 5  ) |\
    /*CKOUT_D4P5_OE             1 bit*/     (      1 << 4  ) |\
    /*CKOUT_D3P5_OE             1 bit*/     (      1 << 3  ) |\
    /*CKOUT_D2P5_OE             1 bit*/     (      1 << 2  ) |\
    /*CKOUT_D1P5_OE             1 bit*/     (      1 << 1  ) |\
    /*CKOUT_D1_OE               1 bit*/     (      1 << 0 ))

#define lrc_pll_con2                               \
    /*SYSPLL_DS(11-0)          12 bit*/     (((192000000/(32000*8))-2)<< 0)

#define lrc_pll_con1                               \
    /*SYSPLL_REFMOD(1-0)        2 bit*/     ((  0b00 << 12 ) |\
    /*SYSPLL_REFDSEN(1-0)       2 bit*/     (   0b01 << 10 ) |\
    /*SYSPLL_REFSEL             1 bit*/     (      1 << 9  ) |\
    /*ref_sel                   2 bit*/     (   0b00 << 7  ) |\
    /*SYSPLL_REFDS(6-0)         7 bit*/     (      0 << 0 ))

#define lrc_pll_con0                                \
    /*SYSPLL_TEST_EN            1 bit*/     ((     0 << 23 ) |\
    /*SYSPLL_TEST_S(1-0)        2 bit*/     (   0b00 << 21 ) |\
    /*SYSPLL_LDO_BYPASS         1 bit*/     (      0 << 20 ) |\
    /*SYSPLL_LDO12D_S(2-0)      3 bit*/     (  0b101 << 17 ) |\
    /*SYSPLL_LDO12A_S(2-0)      3 bit*/     (  0b101 << 14 ) |\
    /*SYSPLL_IVCOS(2-0)         3 bit*/     (  0b011 << 11 ) |\
    /*SYSPLL_LPFR2S(2-0)        3 bit*/     (  0b111 << 8  ) |\
    /*SYSPLL_ICPS(2-0)          3 bit*/     (  0b000 << 5  ) |\
    /*SYSPLL_PFDS(1-0)          2 bit*/     (   0b01 << 3  ) |\
    /*SYSPLL_MODE               1 bit*/     (      0 << 2  ) |\
    /*SYSPLL_RN                 1 bit*/     (      0 << 1  ) |\
    /*SYSPLL_EN                 1 bit*/     (      0 << 0 ))



#endif
#endif
