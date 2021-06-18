#include "asm/includes.h"




/*-----------------------------------------------------------*/
#ifdef SUPPORT_MS_EXTENSIONS
/* #pragma bss_seg(".debug_bss") */
/* #pragma data_seg(".debug_data") */
/* #pragma const_seg(".debug_const") */
/* #pragma code_seg(".debug_code") */
/* #pragma str_literal_override(".debug_code") */
#endif

#define _MPU_WR_SFR_EN 	        do{JL_MPU->WREN = 0xE7;}while(0)
#define _MPU_WR_EN			    JL_MPU->WREN
#define _DEBUG_MSG_CLR			JL_MPU->MSG_CLR

#define _DCU_CON                JL_DCU->CON
#define _DCU_EMU_CON            JL_DCU->EMU_CON
#define _DCU_EMU_MSG            JL_DCU->EMU_MSG
#define _DCU_EMU_ID             JL_DCU->EMU_ID
#define _DCU_CMD_CON            JL_DCU->CMD_CON
#define _DCU_CMD_BEG            JL_DCU->CMD_BEG
#define _DCU_CMD_END            JL_DCU->CMD_END
#define _DCU_CNT_RACK           JL_DCU->CNT_RACK
#define _DCU_CNT_UNACK          JL_DCU->CNT_UNACK

#define _DSP_PC_LIML0			q32DSP(0)->LIM_PC0_L
#define _DSP_PC_LIMH0			q32DSP(0)->LIM_PC0_H
#define _DSP_PC_LIML1			q32DSP(0)->LIM_PC1_L
#define _DSP_PC_LIMH1			q32DSP(0)->LIM_PC1_H
#define _DSP_PC_LIML2			q32DSP(0)->LIM_PC2_L
#define _DSP_PC_LIMH2			q32DSP(0)->LIM_PC2_H
#define _EMU_CON				q32DSP(0)->EMU_CON
#define _EMU_MSG				q32DSP(0)->EMU_MSG
#define _EMU_SSP_H              q32DSP(0)->EMU_SSP_H
#define _EMU_SSP_L              q32DSP(0)->EMU_SSP_L
#define _EMU_USP_H              q32DSP(0)->EMU_USP_H
#define _EMU_USP_L              q32DSP(0)->EMU_USP_L
#define _ETM_CON                q32DSP(0)->ETM_CON
#define _ESU_CON                q32DSP(0)->ESU_CON

#define _ICU_CON                q32DSP_icu(0)->CON
#define _ICU_EMU_CON            q32DSP_icu(0)->EMU_CON
#define _ICU_EMU_MSG            q32DSP_icu(0)->EMU_MSG
#define _ICU_EMU_ID             q32DSP_icu(0)->EMU_ID
#define _ICU_CMD_CON            q32DSP_icu(0)->CMD_CON
#define _ICU_CMD_BEG            q32DSP_icu(0)->CMD_BEG
#define _ICU_CMD_END            q32DSP_icu(0)->CMD_END
#define _ICU_CNT_RACK           q32DSP_icu(0)->CNT_RACK
#define _ICU_CNT_UNACK          q32DSP_icu(0)->CNT_UNACK


#define CDBG_IDx(n, id) ((1<<(n+4)) | (id<<(n*8+8)))
#define CDBG_INV         (1<<7)
#define CDBG_PEN         (1<<3)
#define CDBG_XEN         (1<<2)
#define CDBG_WEN         (1<<1)
#define CDBG_REN         (1<<0)

#define EMU_MISALIGN_EN     (1 << 0)
#define EMU_ILLEGAL_EN      (1 << 1)
#define EMU_DIV0_EN         (1 << 2)
#define EMU_SP_OV_EN        (1 << 3)
#define EMU_PC_LIMIT_EN     (1 << 4)
#define EMU_WP0_ERR_EN      (1 << 8)
#define EMU_FPU_INE_EN      (1 << 16)
#define EMU_FPU_HUGE_EN     (1 << 17)
#define EMU_FPU_TINY_EN     (1 << 18)
#define EMU_FPU_INF_EN      (1 << 19)
#define EMU_FPU_INV_EN      (1 << 20)
#define EMU_DCU_ERR_EN      (1 << 29)
#define EMU_ICU_ERR_EN      (1 << 30)
#define EMU_SYS_ERR_EN      (1 << 31)

#define ETM_PC_TRACE_EN     (1 << 0)
#define ETM_WP_ERR_MODE     (1 << 1)
#define ETM_WP_DREV         (1 << 8)
/*-----------------------------------------------------------*/


extern int text_begin;
extern int text_end;
extern int data_code_pc_limit_begin;
extern int data_code_pc_limit_end;
extern u32 _cpu0_sstack_begin;
extern u32 _cpu0_sstack_end;

static u32 data_magic[16] sec(.data_magic) ;

struct dev_id_str {
    char name[16];
    u8 id ;
};

const struct dev_id_str dev_id_list[] = {
    {"DBG_REV",             0x00},
    {"DBG_ALNK0",           0x01},
    {"DBG_ALNK1",           0x02},
    {"DBG_AUDIO",           0x03},
    {"DBG_SPDIF_D",         0x04},
    {"DBG_SPDIF_I",         0x05},
    {"DBG_ISP",             0x06},
    {"DBG_PAP",             0x07},
    {"DBG_PLNK",            0x08},
    {"DBG_SBC",             0x09},
    {"DBG_AAC",             0x0a},
    {"DBG_AES",             0x0b},
    {"DBG_SD0",             0x0c},
    {"DBG_SD1",             0x0d},
    {"DBG_SPI0",            0x0e},
    {"DBG_SPI1",            0x0f},
    {"DBG_SPI2",            0x10},
    {"DBG_UART0W",          0x11},
    {"DBG_UART0R",          0x12},
    {"DBG_UART1W",          0x13},
    {"DBG_UART1R",          0x14},
    {"DBG_UART2W",          0x15},
    {"DBG_UART2R",          0x16},
    {"DBG_CTM",             0x17},
    {"DBG_AXI_M0",          0x80},
    {"DBG_AXI_M1",          0x81},
    {"DBG_AXI_M2",          0x82},
    {"DBG_AXI_M3",          0x83},
    {"DBG_AXI_M4",          0x84},
    {"DBG_AXI_M5",          0x85},
    {"DBG_AXI_M6",          0x86},
    {"DBG_AXI_M7",          0x87},
    {"DBG_AXI_M8",          0x88},
    {"DBG_AXI_M9",          0x89},
    {"DBG_AXI_MA",          0x8a},
    {"DBG_AXI_MB",          0x8b},
    {"DBG_AXI_MC",          0x8c},
    {"DBG_AXI_MD",          0x8d},
    {"DBG_AXI_ME",          0x8e},
    {"DBG_AXI_MF",          0x8f},
    {"DBG_USB",             0xa0},
//  {"DBG_FM       ",          0xa1},
    {"DBG_BT",              0xa2},
    {"DBG_FFT",             0xa3},
    {"DBG_EQ",              0xa4},
    {"DBG_FIR",             0xa5},
    {"DBG_CPU0_WR",           0xf0},
    {"DBG_CPU0_RD",           0xf1},
    {"DBG_CPU0_IF",           0xf2},
    {"DBG_SDTAP",           0xff},
    {"MSG_NULL",            0xff}
};



static char *const emu_msg[32] = {
    "sys excption",		//31
    "icache excption",		//30
    "dcache excption",		//29
    "reserved",		//28

    "reserved",		//27
    "reserved",		//26
    "reserved",		//25
    "reserved",		//24

    "reserved",		//23
    "reserved",		//22
    "reserved",		//21
    "fpu_inv_err",	//20

    "fpu_inf_err",	//19
    "fpu_tiny",		//18
    "fpu_huge_err",	//17
    "fpu_ine_err",	//16

    "reserved",		//15
    "reserved",		//14
    "reserved",		//13
    "reserved",		//12

    "reserved",		//11
    "reserved",		//10
    "reserved",		//9
    "etm check point 0 err ",		//8

    "reserved",		//7
    "reserved",		//6
    "reserved",		//5
    "pc_limit",		//4

    "stack overflow err",		//3
    "div0_err",		//2
    "illegal_err",	//1
    "misalign_err",	//0
};

static char *const    hcore_emu_msg0[32] = {
    "cpu3 instruction fetch   hmem excption  ",		    //31
    "cpu2 instruction fetch   hmem excption  ",		    //30
    "cpu1 instruction fetch   hmem excption  ",		    //29
    "cpu0 instruction fetch   hmem excption  ",		    //28
    "cpu3 read    hmem excption  ",    //27
    "cpu2 read    hmem excption  ",    //26
    "cpu1 read    hmem excption  ",    //25
    "cpu0 read    hmem excption  ",    //24
    "cpu3 write   hmem excption  ",		    //23
    "cpu2 write   hmem excption  ",		    //22
    "cpu1 write   hmem excption  ",		    //21
    "cpu0 write   hmem excption  ",		    //20
    "reserved",	//19
    "reserved",	//18
    "reserved",	    //17
    "reserved",		    //16

    "reserver ",		    //15
    "reserved",		    //14
    "reserved",	    //13
    "reserver ",		    //12
    "reserved",		    //11
    "reserved",	    //10
    "reserved",	    //9
    "fir access  hmem excption  ",         //8
    "eq  access  hmem excption  ",         //7
    "fft access hmem excption  ",		            //6
    "bt access  hmem excption  ",         //5
    "axi read hmem excption  ",	//4
    "axi write hmem excption  ",	//3
    "lg1 access hmem excption  ",	//2
    "lg0 access hmem excption  ",	    //1
    "sdtap access hmem excption  ",		    // 0

};


static char *const  hcore_emu_msg1[32] = {
    "cpu3 instruction fetch   mmu excption  ",		    //31
    "cpu2 instruction fetch   mmu excption  ",		    //30
    "cpu1 instruction fetch   mmu excption  ",		    //29
    "cpu0 instruction fetch   mmu excption  ",		    //28
    "cpu3 read    mmu excption  ",    //27
    "cpu2 read    mmu excption  ",    //26
    "cpu1 read    mmu excption  ",    //25
    "cpu0 read    mmu excption  ",    //24
    "cpu3 write   mmu excption  ",		    //23
    "cpu2 write   mmu excption  ",		    //22
    "cpu1 write   mmu excption  ",		    //21
    "cpu0 write   mmu excption  ",		    //20
    "reserved",	//19
    "reserved",	//18
    "reserved",	    //17
    "reserved",		    //16

    "reserver ",		    //15
    "reserved",		    //14
    "reserved",	    //13
    "reserver ",		    //12
    "reserved",		    //11
    "reserved",	    //10
    "reserved",	    //9
    "fir access  mmu excption  ",         //8
    "eq  access  mmu excption  ",         //7
    "fft access mmu excption  ",		  //6
    "bt access  mmu excption  ",         //5
    "axi read mmu excption  ",	//4
    "axi write mmu excption  ",	//3
    "lg1 access mmu excption  ",	//2
    "lg0 access mmu excption  ",	    //1
    "sdtap access mmu excption  ",		    // 0
};


static char *const    hcore_emu_msg2[32] = {
    "reserved",		    //31
    "reserved",    //30
    "reserved",		    //29
    "reserved",		    //28
    "reserved",		    //27
    "reserved",	    //26
    "reserved",	    //25
    "reserved",	        //24
    "reserved",         //23
    "reserved",         //22
    "reserved",         //21
    "reserved",		            //20
    "reserved",	//19
    "reserved",	//18
    "reserved",	    //17
    "reserved",		    //16

    "reserver",		    //15
    "reserver",    //14
    "reserver",		    //13
    "reserver",		    //12
    "reserved",		    //11
    "reserved",	    //10
    "reserved",	    //9
    "reserved",         //8
    "reserved",         //7
    "dcu_write invalid",         //6
    "dcu_emu_err",		            //5
    "ilock_err",	//4
    "mpu_err",	//3
    "csfr_read invalid",	//2
    "csfr_write invalid",	    //1
    "hsb emu err ",		    // 0
};

static char *const hsb_emu_msg0[32] = {
    "reserved",		    //31
    "reserved",    //30
    "reserved",		    //29
    "reserved",		    //28
    "reserved",		    //27
    "reserved",	    //26
    "reserved",	    //25
    "reserved",	        //24
    "reserved",         //23
    "reserved",         //22
    "reserved",         //21
    "reserved",		            //20
    "reserved",	//19
    "reserved",	//18
    "reserved",	    //17
    "reserved",		    //16

    "reserved",		    //15
    "reserved",    //14
    "reserved",		    //13
    "reserved",		    //12
    "reserved",		    //11
    "reserved",	    //10
    "reserved",	    //9
    "reserved",	        //8
    "anc ram access error",         //7
    "sbc ram access error",         //6
    "watchdog time out",         //5
    "cpu read axi reserved memory ",	    //4
    "cpu write axi reserved memory ",	    //3
    "cpu read hsb sfr reserved memory ",	    //2
    "cpu write hsb sfr reserved memory ",	    //1
    "lsb emu excption",		    //0
} ;




static char *const lsb_emu_msg0[32] = {

    "reserved",		    //31
    "reserved",    //30
    "reserved",		    //29
    "reserved",		    //28
    "reserved",		    //27
    "reserved",	    //26
    "reserved",	    //25
    "reserved",	        //24
    "reserved",         //23
    "reserved",         //22
    "reserved",         //21
    "reserved",		            //20
    "reserved",	//19
    "reserved",	//18
    "reserved",	    //17
    "reserved",		    //16
    "reserved",		    //15
    "reserved",    //14
    "reserved",		    //13
    "reserved",		    //12
    "reserved",		    //11
    "reserved",	    //10
    "reserved",	    //9
    "reserved",	        //8
    "reserved",         //7
    "reserved",	    //6
    "reserved",	    //5
    "reserved",	        //4
    "reserved",         //3
    "reserved",         //2
    "cpu read lsb sfr reserved memory ",	    //1
    "cpu write lsb sfr reserved memory ",	    //0
} ;


static char *const  icache_emu_msg[32] = {
    "reserver ",            //31
    "reserver ",            //30
    "reserver ",            //29
    "reserver ",            //28
    "reserver ",            //27
    "reserver ",            //26
    "reserver ",            //25
    "reserver ",            //24
    "reserver ",            //23
    "reserver ",            //22
    "reserver ",            //21
    "reserver ",            //20
    "reserver ",            //19
    "reserver ",            //18
    "reserver ",            //17
    "reserver ",            //16
    "reserver ",            //15
    "reserver ",            //14
    "reserver ",            //13
    "reserver ",            //12
    "reserver ",            //11
    "reserver ",            //10
    "reserver ",            //9
    "reserver ",            //8
    "reserver ",            //7
    "reserver ",            //6
    "icache emu error -> icmd_lock_err",  //5
    "icache emu error -> icmd_wkst_err", //4
    "icache emu error -> islv_inv     ", //3
    "icache emu error -> ireq_rack_inv", //2
    "icache emu error -> iway_lock_err", //1
    "icache emu error -> iway_rhit_err", //2

};

static char *const  dcache_emu_msg[32] = {
    "reserver ",            //31
    "reserver ",            //30
    "reserver ",            //29
    "reserver ",            //28
    "reserver ",            //27
    "reserver ",            //26
    "reserver ",            //25
    "reserver ",            //24
    "reserver ",            //23
    "reserver ",            //22
    "reserver ",            //21
    "reserver ",            //20
    "reserver ",            //19
    "reserver ",            //18
    "reserver ",            //17
    "reserver ",            //16
    "reserver ",            //15
    "reserver ",            //14
    "reserver ",            //13
    "reserver ",            //12
    "reserver ",            //11
    "reserver ",            //10
    "reserver ",            //9
    "reserver ",            //8
    "reserver ",            //7
    "reserver ",            //6
    "dcache emu error -> icmd_lock_err",  //5
    "dcache emu error -> icmd_wkst_err", //4
    "dcache emu error -> islv_inv     ", //3
    "dcache emu error -> ireq_rack_inv", //2
    "dcache emu error -> iway_lock_err", //1
    "dcache emu error -> iway_rhit_err", //2

};



static char *get_dev_name(u32 id)
{
    int i ;

    for (i = 0; i < ARRAY_SIZE(dev_id_list); i++) {
        if (dev_id_list[i].id == id) {
            return (char *)dev_id_list[i].name ;
        }
    }
    return NULL;
}

u32 debug_get_dev_id(char *name)
{
    int i ;

    for (i = 0; i < ARRAY_SIZE(dev_id_list); i++) {
        if (!memcmp(dev_id_list[i].name, name, strlen(name))) {
            return dev_id_list[i].id;
        }
    }

    return -1;
}

/********************************* DEBUG PART **********************************/
static void debug_enter_critical()
{
    while (!(_MPU_WR_EN	& BIT(0))) {
        _MPU_WR_SFR_EN;
    }
}

static void debug_exit_critical()
{
    while (_MPU_WR_EN & BIT(0)) {
        _MPU_WR_SFR_EN;
    }
}

static void pc_rang_limit0(void *low_addr, void *high_addr)
{
    _DSP_PC_LIML0 = (u32)low_addr;
    _DSP_PC_LIMH0 = (u32)high_addr;
}

static void pc_rang_limit1(void *low_addr, void *high_addr)
{
    _DSP_PC_LIML1 = (u32)low_addr;
    _DSP_PC_LIMH1 = (u32)high_addr;
}

static void pc_rang_limit2(void *low_addr, void *high_addr)
{
    _DSP_PC_LIML2 = (u32)low_addr;
    _DSP_PC_LIMH2 = (u32)high_addr;
}

/******************************** EMU PART  *****************************/
void debug_emu_misalign_enable(u8 enable)
{
    if (enable) {
        _EMU_CON |= BIT(0);
    } else {
        _EMU_CON &= ~BIT(0);
    }
}

static void debug_emu_stack_limit_set(u8 mode, u32 limit_l, u32 limit_h)
{
    if (mode) {
        _EMU_SSP_H = limit_h;
        _EMU_SSP_L = limit_l;
        /*printf("SSP_H : 0x%x", _EMU_SSP_H);
        printf("SSP_L : 0x%x", _EMU_SSP_L);*/
    } else {
        _EMU_USP_H = limit_h;
        _EMU_USP_L = limit_l;
        /* printf("USP_H : 0x%x", _EMU_USP_H); */
        /* printf("USP_L : 0x%x", _EMU_USP_L); */
    }
}

void debug_etm_pc_trace_enable(u8 enable)
{
    if (enable) {
        _ETM_CON |= BIT(0);
    } else {
        _ETM_CON &= ~BIT(0);
    }
}


/**************************************************/





sec(.volatile_ram_code)
void debug_exception_analyze(unsigned int *sp)
{
#ifdef CONFIG_RELEASE_ENABLE
    P3_PCNT_SET0 = 0xab;
    cpu_reset();
#else

    unsigned int reti = sp[16];
    unsigned int rete = sp[17];
    unsigned int retx = sp[18];
    unsigned int rets = sp[19];
    unsigned int psr  = sp[20];
    unsigned int icfg = sp[21];
    unsigned int usp  = sp[22];
    unsigned int ssp  = sp[23];

    int i;
    extern int _stack_end;

    puts("\n\n---------system--exception---------\n");

    printf("PC: 0x%x --> 0x%x --> 0x%x --> 0x%x\n",
           q32DSP(0)->ETM_PC3, q32DSP(0)->ETM_PC2,
           q32DSP(0)->ETM_PC1, q32DSP(0)->ETM_PC0);

    printf("usp   : 0x%08x\n", usp);
    printf("ssp   : 0x%08x\n", ssp);
    printf("sp    : 0x%08x\n", (u32)sp);
    printf("Stack : 0x%08x\n", (u32)&_stack_end);
    printf("rets  : 0x%08x\n", rets);
    printf("reti  : 0x%08x\n", reti);
    printf("retx  : 0x%08x\n", retx);
    printf("rete  : 0x%08x\n", rete);
    printf("psr   : 0x%08x\n", psr);
    printf("icfg  : 0x%08x\n", icfg);

    printf("EMU_CON = 0x%x\n", _EMU_CON) ;
    printf("EMU_MSG = 0x%x\n", _EMU_MSG) ;

    //EMU query sequence
    // CPU -> HCORE -> HSB -> LSB
    for (i = 0; i < 32; i++) {
        if (_EMU_MSG & BIT(i)) {
            printf("\n[0-CPU] emu err msg : %s", emu_msg[31 - i]);
        }
    }

    //system
    if (_EMU_MSG & BIT(31)) {
        for (i = 0; i < 32; i++) {
            if (JL_CEMU->MSG0 & BIT(i)) {
                printf("\n[1-HCORE] hmem err msg : %s", hcore_emu_msg0[31 - i]);
                if ((i == 4)  || (i == 3)  || (i == 2) || (i == 1)) {
                    printf("	at dev %s\n", get_dev_name(JL_CEMU->ID));
                }
            }
        }

        for (i = 0; i < 32; i++) {
            if (JL_CEMU->MSG1 & BIT(i)) {
                printf("\n[1-HCORE] mmu err msg : %s", hcore_emu_msg1[31 - i]);
                if ((i == 4) || (i == 3) || (i == 2) || (i == 1)) {
                    printf("	at dev  %s\n", get_dev_name(JL_CEMU->ID));
                }
            }
        }

        for (i = 0; i < 32; i++) {
            if (JL_CEMU->MSG2 & BIT(i)) {
                printf("\n[1-HCORE] emu err msg : %s", hcore_emu_msg2[31 - i]);
                if ((i == 6) || (i == 2) || (i == 1)) {
                    printf("	at dev %s\n", get_dev_name(JL_CEMU->ID)) ;
                } else if (i == 3) {
                    printf("	mpu limit : %x", (JL_CEMU->ID & 0xf00) >> 8);
                    printf("	at dev  : ID = %x, %s\n", JL_CEMU->ID & 0xff,
                           get_dev_name(JL_CEMU->ID & 0xff));
                }
            }
        }

        //HSB
        if (JL_CEMU->MSG2 & BIT(0)) {
            for (i = 0; i < 32; i++) {
                if (JL_HEMU->MSG0 & BIT(i)) {
                    printf("\n[2-HSB] emu err msg : %s", hsb_emu_msg0[31 - i]);
                    if ((i == 1) || (i == 2) || (i == 3) ||
                        (i == 4) || (i == 6) || (i == 7)) {
                        printf("	at dev %s\n", get_dev_name(JL_HEMU->ID));
                    }
                }
            }

            //LSB
            if (JL_HEMU->MSG0 & BIT(0)) {
                for (i = 0; i < 32; i++) {
                    if (JL_LEMU->MSG0 & BIT(i)) {
                        printf("\n[3-LSB] emu err msg : %s", lsb_emu_msg0[31 - i]);
                        printf("	at dev %s\n", get_dev_name(JL_LEMU->ID));
                    }
                }
            }
        }
    }

    if (_EMU_MSG & BIT(30)) {
        for (i = 0 ; i < 32 ; i++) {
            if (_ICU_EMU_MSG & BIT(i)) {
                printf("\n[X-ICACHE] emu err msg : %s\n", icache_emu_msg[31 - i]);
                if (i == 3) {
                    printf("	at dev %s\n", get_dev_name(_ICU_EMU_ID)) ;
                }
            }
        }
    }

    if (_EMU_MSG & BIT(29)) {
        for (i = 0 ; i < 32 ; i++) {
            if (_DCU_EMU_MSG & BIT(i)) {
                printf("\n[Y-DCACHE] emu err msg : %s", dcache_emu_msg[31 - i]);
                if (i == 3) {
                    printf("	at dev %s\n", get_dev_name(_DCU_EMU_ID)) ;
                }
            }
        }
    }


    putchar('\n');
    for (i = 0; i < 16; ++i) {
        printf("R%d = 0x%08x\n", i, sp[i]);
    }

    for (i = 0; i < ARRAY_SIZE(data_magic); i++) {
        if (data_magic[i] != 0xffffffff) {
            printf("data_magic: %d, 0x%08x\n", i, data_magic[i]);
        }
    }

    while (1);
#endif

}

static void mpu_privilege(int idx, u8 type, u8 did, u8 x, u8 r, u8 w)
{
    u8 i;

    switch (type) {
    case 'C':
        /*printf("C[%d] : x(%d) : r(%d) : w(%d)", idx, x, r, w);*/
        JL_MPU->CON[idx] |= ((x) ? MPU_XEN : 0) | ((r) ? MPU_REN : 0) | ((w) ? MPU_WEN : 0);
        break;
    case 'P':
        /*printf("P[%d] : r(%d) : w(%d)", idx, r, w);*/
        JL_MPU->CON[idx] |= ((r) ? MPU_PREN : 0) | ((w) ? MPU_PWEN : 0);
        break;
    case '0':
    case '1':
    case '2':
        i = type - '0';
        /*printf("PID[%d][%d] : did 0x%x : r(%d) : w(%d)", idx, i, did, r, w);*/
        JL_MPU->CID[idx] |= MPU_IDx_cfg(i, did);
        JL_MPU->CON[idx] |= MPU_IDx_pen(i, ((r) ? 1 : 0), ((w) ? 1 : 0));
        break;
    default:
        break;
    }
}

// [MPU format]
// begin | end | privilege | inv | pid0_privilege |
static int __parser(int idx, const char *format, va_list argptr)
{
    u8 type = 0;
    u8 did = 0;
    u8 privilege = 0;

    while (*format) {
        switch (*format) {
        case 'C':
            mpu_privilege(idx, type, did, privilege & BIT(2), privilege & BIT(1), privilege & BIT(0));
            did = 0;
            type = *format;
            privilege = 0;
            break;
        case 'P':
            mpu_privilege(idx, type, did, privilege & BIT(2), privilege & BIT(1), privilege & BIT(0));
            did = 0;
            type = *format;
            privilege = 0;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
            mpu_privilege(idx, type, did, privilege & BIT(2), privilege & BIT(1), privilege & BIT(0));
            did = va_arg(argptr, int);
            type = *format;
            privilege = 0;
            break;

        case 'w':
            privilege |= BIT(0);
            break;
        case 'r':
            privilege |= BIT(1);
            break;
        case 'x':
            privilege |= BIT(2);
            break;

        default:
            break;
        }
        format++;
    }
    mpu_privilege(idx, type, did, privilege & BIT(2), privilege & BIT(1), privilege & BIT(0));

    return 1;
}

#define MPU_MAX_SIZE    4

void debug_mpu_set(int idx, u32 begin, u32 end, u32 inv, const char *format, ...)
{
    if (idx > (MPU_MAX_SIZE - 1)) {
        printf("MPU idx overflow %d", idx);
        return;
    }

    debug_enter_critical();

    JL_MPU->BEG[idx] = begin;
    JL_MPU->END[idx] = end;

    JL_MPU->CON[idx] = 0;
    JL_MPU->CID[idx] = 0;

    if (inv) {
        JL_MPU->CON[idx] = MPU_INV ;
    }

    va_list argptr;
    va_start(argptr, format);

    __parser(idx, format, argptr);

    va_end(argptr);

    debug_exit_critical();

    //echo
    /*printf("JL_MPU->CON[%d] 0x%x", idx, JL_MPU->CON[idx]);
    printf("JL_MPU->CID[%d] 0x%x", idx, JL_MPU->CID[idx]);
    printf("JL_MPU->BEG[%d] 0x%x", idx, JL_MPU->BEG[idx]);
    printf("JL_MPU->END[%d] 0x%x", idx, JL_MPU->END[idx]);*/

}


void debug_ram_limit_set(u32 num, u32 begin, u32 end, u32 con, u32  id, u32 inv)
{
    if (num > 3) {
        printf("MPU idx overflow %d", num);
        return;
    }

    debug_enter_critical();

    JL_MPU->BEG[num] = begin ;
    JL_MPU->END[num]  = end ;
    JL_MPU->CID[num] = id ;

    if (inv == 1) {
        JL_MPU->CON[num] |= con | MPU_INV ;
    } else {

        JL_MPU->CON[num] |= con ;
    }

    debug_exit_critical();
}



void debug_init()
{
    _EMU_CON = EMU_MISALIGN_EN | EMU_ILLEGAL_EN | EMU_DIV0_EN | EMU_PC_LIMIT_EN |
#ifdef CONFIG_FLOAT_DEBUG_ENABLE
               EMU_FPU_INV_EN  | EMU_FPU_INF_EN  |
#endif
               EMU_SYS_ERR_EN;

    _ETM_CON = ETM_PC_TRACE_EN;

    //设置中断堆栈溢出保护（supermode）
    debug_emu_stack_limit_set(1, (u32)&_cpu0_sstack_begin, (u32)&_cpu0_sstack_end);

    _EMU_CON |= EMU_SP_OV_EN;

    /*printf("JL_CEMU->CON0 : 0x%x", JL_CEMU->CON0);
    printf("JL_CEMU->CON1 : 0x%x", JL_CEMU->CON1);
    printf("JL_CEMU->CON2 : 0x%x", JL_CEMU->CON2);

    printf("JL_HEMU->CON0 : 0x%x", JL_HEMU->CON0);
    printf("JL_HEMU->CON1 : 0x%x", JL_HEMU->CON1);
    printf("JL_HEMU->CON2 : 0x%x", JL_HEMU->CON2);

    printf("JL_LEMU->CON0 : 0x%x", JL_LEMU->CON0);
    printf("JL_LEMU->CON1 : 0x%x", JL_LEMU->CON1);
    printf("JL_LEMU->CON2 : 0x%x", JL_LEMU->CON2);

    printf("_ICU_EMU_CON : 0x%x", _ICU_EMU_CON);
    printf("_DCU_EMU_CON : 0x%x", _DCU_EMU_CON);*/

    ////maskrom code range
    pc_rang_limit2((void *)0x100000, (void *)0x110000);

    //ram code
    pc_rang_limit1(&data_code_pc_limit_begin, &data_code_pc_limit_end);

    //flash code range
    pc_rang_limit0(&text_begin, &text_end);

    //_EMU_CON |= BIT(4) ;

    /*printf("pc_limit0: 0x%x ~ 0x%x",  _DSP_PC_LIML0 & 0x1ffffff, _DSP_PC_LIMH0 & 0x1ffffff);
    printf("pc_limit1: 0x%x ~ 0x%x",  _DSP_PC_LIML1 & 0x1ffffff, _DSP_PC_LIMH1 & 0x1ffffff);
    printf("pc_limit2: 0x%x ~ 0x%x",  _DSP_PC_LIML2 & 0x1ffffff, _DSP_PC_LIMH2 & 0x1ffffff);*/

    memset(data_magic, 0xff, sizeof(data_magic));

    //RAM code
    debug_mpu_set(0, (u32)&data_code_pc_limit_begin, (u32)&data_code_pc_limit_end - 1, 0, "Cxr");
}



void debug_disable()
{
    _EMU_CON = 0;
}



